#include "dxfio/load.hpp"

#include "disk.hpp"
#include "header/io.hpp"
#include "past.hpp"
#include "written.hpp"

#include "dxfio/edit/add.hpp"
#include "dxfio/save.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <variant>
#include <vector>

namespace DxfIO
{
namespace
{
auto Reseeded(std::string_view value) -> std::string
{
  std::string text = Acad("empty-r2018.dxf");
  constexpr std::string_view seed = "$HANDSEED\r\n";
  const std::size_t at = text.find(seed) + seed.size();
  const std::size_t end = text.find("  9\r\n", at);
  text.replace(at, end - at, value);
  return text;
}

auto Seed(const Document &drawing) -> std::vector<Group>
{
  const auto found = std::ranges::find(drawing.Header, "$HANDSEED", &Header::Variable::Name);
  REQUIRE(found != drawing.Header.end());
  return found->Values;
}
} // namespace

TEST_CASE("load: a $HANDSEED not past every handle is raised past them, and told")
{
  auto read = Load(Reseeded("  5\r\n2\r\n"));
  REQUIRE(read);
  const Handle past = Past(read->Document);
  CHECK(past > Handle{2});
  CHECK(Seed(read->Document) == std::vector{Group{.Code = 5, .Value = past}});
  REQUIRE(read->Issues.size() == 1);
  CHECK(read->Issues.front().What ==
        std::format("$HANDSEED 2 is not past the handles in use, raised to {}", Hex(past)));
  const auto added = Edit::Add(read->Document, Entities::Line{});
  REQUIRE(added);
  CHECK(*added == past);
}

TEST_CASE("load: a $HANDSEED past every handle, null, empty or no handle is left as it is")
{
  const auto fine = Load(Acad("empty-r2018.dxf"));
  REQUIRE(fine);
  CHECK(fine->Issues.empty());
  for (const std::string_view value : {"  5\r\n0\r\n", "  1\r\nZZ\r\n", ""})
  {
    CAPTURE(value);
    const auto read = Load(Reseeded(value));
    REQUIRE(read);
    CHECK(read->Issues.empty());
    CHECK(Seed(read->Document).size() == (value.empty() ? 0U : 1U));
  }
}

TEST_CASE("load: a sample drawing read whole, its sections in their order")
{
  const auto read = Load(Bytes(std::filesystem::path{DXFIO_FUZZ_DIR} / "corpus" / "sample-r2018.dxf"));
  REQUIRE(read);
  const Document &drawing = read->Document;
  CHECK(drawing.Order == std::vector<std::string>{"HEADER", "CLASSES", "TABLES", "BLOCKS", "ENTITIES", "OBJECTS"});
  CHECK(Header::Declared(drawing.Header) == Version::R2018);
  CHECK(drawing.Layer.Records.size() > 0);
  CHECK(drawing.Appid.Head.Name == "APPID");
  CHECK(drawing.Ltype.Records.size() > 0);
  CHECK(drawing.Loose.empty());
  CHECK_FALSE(drawing.Entities.empty());
  CHECK_FALSE(drawing.Objects.empty());
}

TEST_CASE("load: text decoded as the release and the page say")
{
  const auto read = Load("  0\nSECTION\n  2\nHEADER\n  9\n$ACADVER\n  1\n"
                         "AC1015\n  9\n$DWGCODEPAGE\n  3\nANSI_1252\n"
                         "  0\nENDSEC\n  0\nSECTION\n  2\nENTITIES\n"
                         "  0\nTEXT\n  1\ncaf\xe9\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 1);
  CHECK(std::get<Entities::Text>(read->Document.Entities.front()).Lettering.Value == "caf\xc3\xa9");
}

TEST_CASE("load: a section named through an escape is not the one it spells")
{
  const auto read = Load("  0\nSECTION\n  2\n\\U+0045NTITIES\n  0\nLINE\n"
                         "  5\n2A\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  CHECK(read->Document.Entities.empty());
  REQUIRE(read->Document.Sections.size() == 1);
  CHECK(read->Document.Sections.front().Groups.size() == 2);
  const auto saved = Tests::Written(read->Document, Options{});
  REQUIRE(saved);
  CHECK(saved->contains("\\U+0045NTITIES"));
  CHECK(saved->contains("2A"));
}

TEST_CASE("load: a section given again is dropped, the first kept")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n  0\nENDSEC\n"
                         "  0\nSECTION\n  2\nENTITIES\n  0\nCIRCLE\n"
                         "  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  CHECK(read->Document.Order == std::vector<std::string>{"ENTITIES"});
  REQUIRE(read->Document.Entities.size() == 1);
  CHECK(std::holds_alternative<Entities::Line>(read->Document.Entities.front()));
  REQUIRE(read->Issues.size() == 1);
  CHECK(read->Issues.front().What == "section ENTITIES given again, the first kept");
}

TEST_CASE("load: what is read past is told, what stops the file handed back")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n"
                         "1001\nACAD\n1001\nACAD\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  CHECK(read->Issues.size() == 1);
  CHECK(Load("  0\nLINE\n") == std::unexpected{Failure{1, "SECTION expected, found group 0"}});
}

TEST_CASE("load: a value that does not parse is left out and told, the record read without it")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n 39\nabc\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 1);
  const auto *line = std::get_if<Entities::Line>(&read->Document.Entities.front());
  REQUIRE(line != nullptr);
  CHECK(line->Thickness == 0.0);
  CHECK(read->Issues == std::vector{Issue{.Line = 8, .What = "number expected for group 39, found \"abc\", left out"}});
}

TEST_CASE("load: an integer past the range of its group is left out and told, so what loads saves")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n 62\n70000\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 1);
  const auto *line = std::get_if<Entities::Line>(&read->Document.Entities.front());
  REQUIRE(line != nullptr);
  CHECK(line->Record.Color == 256);
  CHECK(read->Issues ==
        std::vector{Issue{.Line = 8, .What = "integer of 16 bits expected for group 62, found \"70000\", left out"}});
  CHECK(Tests::Written(read->Document, {.Release = Version::R12}));
}

TEST_CASE("load: a chunk that is not hex is left out and told, so what loads saves in binary too")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n1001\nACAD\n1004\nzz\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 1);
  CHECK(BaseOf(read->Document.Entities.front()).Extended.front().Groups.empty());
  CHECK(read->Issues == std::vector{Issue{.Line = 10, .What = "hex expected for group 1004, found \"zz\", left out"}});
  CHECK(Tests::Written(read->Document, {.Release = Version::R2000, .Binary = true}));
}

TEST_CASE("load: a blank after a section word or a record name is trimmed and told")
{
  const auto read = Load("  0\nSECTION \n  2\nENTITIES\n  0\nLINE \n  0\nENDSEC \n  0\nEOF \n");
  REQUIRE(read);
  CHECK(read->Document.Order == std::vector<std::string>{"ENTITIES"});
  REQUIRE(read->Document.Entities.size() == 1);
  CHECK(std::holds_alternative<Entities::Line>(read->Document.Entities.front()));
  CHECK(read->Issues.size() == 4);
  CHECK(read->Issues.front() == Issue{.Line = 2, .What = "group 0 holds \"SECTION \" with blanks, trimmed"});
}

TEST_CASE("load: text after EOF is left out and told")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nENDSEC\n  0\nEOF\ngarbage\n");
  REQUIRE(read);
  CHECK(read->Document.Order == std::vector<std::string>{"ENTITIES"});
  CHECK(read->Issues == std::vector{Issue{.Line = 9, .What = "what follows EOF, left out"}});
}

TEST_CASE("load: a file cut short keeps what it read, its last section closed at the end, told")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n 62\n");
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 1);
  CHECK(std::holds_alternative<Entities::Line>(read->Document.Entities.front()));
  CHECK(read->Issues == std::vector{
                            Issue{.Line = 7, .What = "group 62 at the end of the file has no value, left out"},
                            Issue{.Line = 1, .What = "ENDSEC of ENTITIES missing, closed at the end of the file"},
                        });
}

TEST_CASE("load: tables outside a TABLES section are read as one, which the save then writes")
{
  const auto read = Load("  0\nSECTION\n  2\nHEADER\n  9\n$ACADVER\n  1\nAC1009\n  0\nENDSEC\n"
                         "  0\nTABLE\n  2\nLAYER\n 70\n     1\n  0\nLAYER\n  2\nWALLS\n 70\n     0\n"
                         " 62\n     1\n  6\nCONTINUOUS\n  0\nENDTAB\n"
                         "  0\nSECTION\n  2\nENTITIES\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  CHECK(read->Document.Order == std::vector<std::string>{"HEADER", "TABLES", "ENTITIES"});
  REQUIRE(read->Document.Layer.Records.size() == 1);
  CHECK(read->Document.Layer.Records.front().Record.Name == "WALLS");
  CHECK(read->Issues == std::vector{Issue{.Line = 11, .What = "TABLE outside a TABLES section, taken as one"}});
  const auto saved = Tests::Written(read->Document, {.Release = Version::R12});
  REQUIRE(saved);
  CHECK(saved->contains("SECTION\r\n  2\r\nTABLES\r\n  0\r\nTABLE\r\n"));
}

TEST_CASE("load: with no $ACADVER, records carrying subclass markers read as R2000, told")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nLWPOLYLINE\n  5\nA1\n100\nAcDbEntity\n  8\n0\n"
                         "100\nAcDbPolyline\n 90\n        2\n 70\n     0\n 10\n0.0\n 20\n0.0\n 10\n1.0\n 20\n0.0\n"
                         "  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 1);
  CHECK(std::holds_alternative<Entities::LwPolyline>(read->Document.Entities.front()));
  CHECK(Header::Declared(read->Document.Header) == Version::R2000);
  CHECK(read->Document.Header.front() ==
        Header::Variable{.Name = "$ACADVER", .Values = {Group{.Code = 1, .Value = std::string{"AC1015"}}}});
  CHECK(read->Issues ==
        std::vector{Issue{.Line = 0, .What = "no $ACADVER, read as R2000 for the subclass markers its records carry"}});
  const auto headed = Load("  0\nSECTION\n  2\nHEADER\n  9\n$INSUNITS\n 70\n     4\n  0\nENDSEC\n"
                           "  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n100\nAcDbEntity\n  8\n0\n100\nAcDbLine\n"
                           "  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(headed);
  REQUIRE(headed->Document.Header.size() == 2);
  CHECK(headed->Document.Header.front().Name == "$ACADVER");
  CHECK(headed->Document.Header.back().Name == "$INSUNITS");
}

TEST_CASE("load: a file declared R12 keeps every kind R12 does not have as Unknown, markers or not")
{
  std::string text = Bytes(std::filesystem::path{DXFIO_FUZZ_DIR} / "corpus" / "sample-r2018.dxf");
  text.replace(text.find("AC1032"), 6, "AC1009");
  const auto read = Load(text);
  REQUIRE(read);
  CHECK(std::ranges::all_of(read->Document.Objects,
                            [](const Object &one) -> bool { return std::holds_alternative<Unknown>(one); }));
  CHECK(std::ranges::any_of(read->Document.Entities,
                            [](const Entity &one) -> bool { return std::holds_alternative<Unknown>(one); }));
  CHECK(std::ranges::any_of(read->Document.Entities,
                            [](const Entity &one) -> bool { return std::holds_alternative<Entities::Line>(one); }));
}

TEST_CASE("load: with no $ACADVER and no subclass marker, the file is R12, as it was")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n  8\n0\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  CHECK(Header::Declared(read->Document.Header) == Version::R12);
  CHECK(read->Document.Header.empty());
  CHECK(read->Issues.empty());
}

TEST_CASE("load: a code page named as DWG names it reads its text")
{
  const auto read = Load("  0\nSECTION\n  2\nHEADER\n  9\n$ACADVER\n  1\nAC1015\n  9\n$DWGCODEPAGE\n  3\nksc5601\n"
                         "  0\nENDSEC\n  0\nSECTION\n  2\nENTITIES\n  0\nTEXT\n100\nAcDbEntity\n  8\n0\n"
                         "100\nAcDbText\n  1\n\xb0\xa1\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  CHECK(read->Issues.empty());
  REQUIRE(read->Document.Entities.size() == 1);
  const auto *text = std::get_if<Entities::Text>(&read->Document.Entities.front());
  REQUIRE(text != nullptr);
  CHECK(text->Lettering.Value == "\xea\xb0\x80");
}

TEST_CASE("load: a group of a code DXF does not define is kept with the record, and told")
{
  const auto read = Load("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n250\n2\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  REQUIRE(read->Document.Entities.size() == 1);
  CHECK(BaseOf(read->Document.Entities.front()).Extra == std::vector{Group{.Code = 250, .Value = std::string{"2"}}});
  CHECK(read->Issues == std::vector{Issue{.Line = 7, .What = "group 250 is no DXF group, kept"}});
}

TEST_CASE("load: a section whose parts do not hold together stops the file")
{
  CHECK(Load("  0\nSECTION\n  2\nHEADER\n  1\nAC1032\n  0\nENDSEC\n  0\nEOF\n") ==
        std::unexpected{Failure{5, "header variable expected, found group 1"}});
  CHECK(Load("  0\nSECTION\n  2\nENTITIES\n  0\nENDSEC\n"
             "  0\nSECTION\n  2\nTABLES\n  0\nTABLE\n  2\nLAYER\n  0\nENDSEC\n  0\nEOF\n") ==
        std::unexpected{Failure{13, "ENDTAB missing"}});
}

namespace
{
// Two thousand R12 entities laid so that the cuts between the parts read on threads fall on an ATTRIB, a
// VERTEX, a SEQEND, plain lines and the end: 250 lines, an INSERT with 400 attributes across two cuts, a
// POLYLINE with 300 vertices across one, lines, an INSERT whose SEQEND stands on a cut, lines, and an INSERT
// whose attributes run from the last cut to the end; each line gives its layer twice so that every one tells
// an issue. Then six hundred objects.
auto Spanning() -> std::string
{
  std::string out = "  0\nSECTION\n  2\nENTITIES\n";
  const auto lines = [&out](int from, int to) -> void
  {
    for (int at = from; at < to; ++at)
    {
      out += std::format("  0\nLINE\n  8\nL{}\n  8\nM{}\n 10\n{}\n 20\n0.0\n 11\n1.0\n 21\n{}\n", at, at, at, at);
    }
  };
  const auto insert = [&out](int attributes) -> void
  {
    out += "  0\nINSERT\n  8\nI\n 66\n1\n  2\nBLOCK\n 10\n0.0\n 20\n0.0\n";
    for (int at = 0; at < attributes; ++at)
    {
      out += std::format("  0\nATTRIB\n  8\nI\n 10\n{}\n 20\n0.0\n 40\n1.0\n  1\nv{}\n  2\nT{}\n", at, at, at);
    }
    out += "  0\nSEQEND\n  8\nI\n";
  };
  lines(0, 250);
  insert(400);
  out += "  0\nPOLYLINE\n  8\nP\n 66\n1\n 70\n0\n";
  for (int at = 0; at < 300; ++at)
  {
    out += std::format("  0\nVERTEX\n  8\nP\n 10\n{}\n 20\n{}\n", at, at * 2);
  }
  out += "  0\nSEQEND\n  8\nP\n";
  lines(250, 396);
  insert(43);
  lines(396, 951);
  insert(298);
  out += "  0\nENDSEC\n  0\nSECTION\n  2\nOBJECTS\n";
  for (int at = 0; at < 600; ++at)
  {
    out += std::format("  0\nXRECORD\n  5\n{:X}\n280\n1\n  1\nx{}\n", 0x1000 + at, at);
  }
  return out + "  0\nENDSEC\n  0\nEOF\n";
}
} // namespace

TEST_CASE("load: a drawing read on several threads is the drawing read on one, issues and all")
{
  const std::string text = Spanning();
  const auto alone = Load(text, Loading{.Threads = 1});
  REQUIRE(alone);
  REQUIRE(alone->Document.Entities.size() == 955);
  CHECK(alone->Document.Objects.size() == 600);
  CHECK(alone->Issues.size() == 951);
  const auto *insert = std::get_if<Entities::Insert>(&alone->Document.Entities.at(250));
  REQUIRE(insert != nullptr);
  CHECK(insert->Attributes.Children.size() == 400);
  const auto *polyline = std::get_if<Entities::Polyline>(&alone->Document.Entities.at(251));
  REQUIRE(polyline != nullptr);
  CHECK(polyline->Vertices.Children.size() == 300);
  for (const int threads : {0, 2, 8})
  {
    CAPTURE(threads);
    const auto together = Load(text, Loading{.Threads = threads});
    REQUIRE(together);
    CHECK(together->Document == alone->Document);
    CHECK(together->Issues == alone->Issues);
  }
  std::istringstream from{text};
  const auto streamed = Load(from, Loading{.Threads = 4});
  REQUIRE(streamed);
  CHECK(streamed->Document == alone->Document);
}

TEST_CASE("load: a drawing reads from a stream and from a file")
{
  const std::filesystem::path file = std::filesystem::path{DXFIO_FUZZ_DIR} / "corpus" / "sample-r2018.dxf";
  const auto whole = Load(Bytes(file));
  REQUIRE(whole);
  std::istringstream from{Bytes(file)};
  const auto streamed = Load(from);
  REQUIRE(streamed);
  CHECK(streamed->Document == whole->Document);
  const auto filed = LoadFile(file);
  REQUIRE(filed);
  CHECK(filed->Document == whole->Document);
}

TEST_CASE("load: a file whose size is not known is read to its end")
{
  const std::filesystem::path device{"/dev/null"};
  if (!std::filesystem::exists(device))
  {
    return;
  }
  std::error_code unsized;
  CHECK(std::filesystem::file_size(device, unsized) == static_cast<std::uintmax_t>(-1));
  CHECK(unsized);
  const auto read = LoadFile(device);
  REQUIRE(read);
  CHECK(read->Document == Document{});
}

TEST_CASE("load: a stream that fails, or a file not there, is told")
{
  std::istringstream broken{"0\nEOF\n"};
  broken.setstate(std::ios::badbit);
  const auto unread = Load(broken);
  REQUIRE_FALSE(unread);
  CHECK(unread.error().What == "the stream could not be read");
  const std::filesystem::path missing = std::filesystem::path{DXFIO_FUZZ_DIR} / "missing.dxf";
  const auto absent = LoadFile(missing);
  REQUIRE_FALSE(absent);
  CHECK(absent.error().What == missing.string() + " cannot be read");
}

TEST_CASE("load: a stream that failed before it was read is told")
{
  std::istringstream failed{"  0\nEOF\n"};
  failed.setstate(std::ios::failbit);
  const auto unread = Load(failed);
  REQUIRE_FALSE(unread);
  CHECK(unread.error().What == "the stream could not be read");
  std::ifstream closed{std::filesystem::path{DXFIO_FUZZ_DIR} / "missing.dxf"};
  CHECK_FALSE(Load(closed));
}
} // namespace DxfIO
