#include "dxfio/save.hpp"

#include "../fuzz/oracle.hpp"
#include "disk.hpp"
#include "header/io.hpp"
#include "tags/sentinel.hpp"
#include "written.hpp"

#include "dxfio/create.hpp"
#include "dxfio/edit/add.hpp"
#include "dxfio/load.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <ios>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO
{
namespace
{
const std::filesystem::path Fuzz{DXFIO_FUZZ_DIR};

auto Entries(const Document &drawing) -> std::size_t
{
  std::size_t out = 0;
  for (const Object &one : drawing.Objects)
  {
    const auto *names = std::get_if<Objects::Dictionary>(&one);
    out += names == nullptr ? 0 : names->Entries.size();
  }
  return out;
}
} // namespace

TEST_CASE("save: up, an unknown object with no handle leaves every dictionary whole")
{
  Document drawing = Create(Version::R2000);
  drawing.Objects.emplace_back(Unknown{.Type = Symbol{"FOO"}, .Base = {}, .Groups = {}});
  const auto older = Tests::Written(drawing, Options{.Release = Version::R2000});
  REQUIRE(older);
  const auto read = Load(*older);
  REQUIRE(read);
  const auto newer = Tests::Written(read->Document, Options{.Release = Version::R2004});
  REQUIRE(newer);
  const auto again = Load(*newer);
  REQUIRE(again);
  CHECK(Entries(read->Document) > 0);
  CHECK(Entries(again->Document) == Entries(read->Document));
}

TEST_CASE("save: every sample drawing saves to a fixed point, nothing lost")
{
  for (const auto &path : Files(Fuzz / "corpus"))
  {
    CAPTURE(path.filename().string());
    const std::string text = Bytes(path);
    CHECK(Broken(text).empty());
    const auto first = Load(text);
    REQUIRE(first);
    const Options with{.Release = Header::Declared(first->Document.Header)};
    const auto once = Tests::Written(first->Document, with);
    REQUIRE(once);
    const auto second = Load(*once);
    REQUIRE(second);
    CHECK(second->Document == first->Document);
  }
}

TEST_CASE("save: every drawing AutoCAD wrote reads whole and saves the same")
{
  for (const auto &path : Files(Acad()))
  {
    CAPTURE(path.filename().string());
    const std::string text = Bytes(path);
    CHECK(Broken(text).empty());
    const auto first = Load(text);
    REQUIRE(first);
    CHECK(first->Issues.empty());
    const Options with{.Release = Header::Declared(first->Document.Header)};
    const auto once = Tests::Written(first->Document, with);
    REQUIRE(once);
    const auto second = Load(*once);
    REQUIRE(second);
    CHECK(second->Document == first->Document);
  }
}

TEST_CASE("save: a page not carried stays named, its text the same bytes")
{
  constexpr std::string_view page = "$DWGCODEPAGE\r\n  3\r\n";
  std::string text = Bytes(Acad() / "texts-r2000.dxf");
  text.replace(text.find(page) + page.size(), 9, "DOS850");
  const std::string chinese = "\xd6\xd0\x81^\xce\xc4";
  const std::string left = "\r\nleft\r\n";
  text.replace(text.find(left), left.size(), "\r\n" + chinese + "\r\n");
  const auto read = Load(text);
  REQUIRE(read);
  CHECK(read->Issues.size() == 1);
  const auto same = Tests::Written(read->Document, Options{.Release = Version::R2000});
  REQUIRE(same);
  CHECK(same->contains("  3\r\nDOS850\r\n"));
  CHECK(same->contains(chinese));
  const auto chosen = Tests::Written(read->Document, Options{.Release = Version::R2000, .Codepage = "ANSI_1252"});
  REQUIRE(chosen);
  CHECK(chosen->contains("  3\r\nANSI_1252\r\n"));
}

TEST_CASE("save: an East Asian page read as its characters, written as its bytes")
{
  constexpr std::string_view page = "$DWGCODEPAGE\r\n  3\r\n";
  std::string text = Bytes(Acad() / "texts-r2000.dxf");
  text.replace(text.find(page) + page.size(), 9, "ANSI_936");
  const std::string chinese = "\xd6\xd0\xce\xc4";
  const std::string left = "\r\nleft\r\n";
  text.replace(text.find(left), left.size(), "\r\n" + chinese + "\r\n");
  const auto read = Load(text);
  REQUIRE(read);
  CHECK(read->Issues.empty());
  const auto same = Tests::Written(read->Document, Options{.Release = Version::R2000});
  REQUIRE(same);
  CHECK(same->contains("\r\n" + chinese + "\r\n"));
  const auto wide = Tests::Written(read->Document, Options{.Release = Version::R2018});
  REQUIRE(wide);
  CHECK(wide->contains("\r\n\u4e2d\u6587\r\n"));
}

TEST_CASE("save: down, a variable the release has not is left out")
{
  const auto read = Load(Bytes(Acad() / "empty-r2018.dxf"));
  REQUIRE(read);
  const auto older = Tests::Written(read->Document, Options{.Release = Version::R2000});
  REQUIRE(older);
  CHECK(older->contains("$ACADVER\r\n  1\r\nAC1015\r\n"));
  CHECK_FALSE(older->contains("$LASTSAVEDBY"));
  CHECK_FALSE(older->contains("$DIMTXTDIRECTION"));
}

TEST_CASE("save: a variable whose group changes takes that release's value")
{
  const auto read = Load(Bytes(Acad() / "empty-r2018.dxf"));
  REQUIRE(read);
  CHECK(read->Document.Header.size() > 200);
  const auto older = Tests::Written(read->Document, Options{.Release = Version::R2004});
  REQUIRE(older);
  CHECK(older->contains("$XCLIPFRAME\r\n290\r\n     0\r\n"));
}

TEST_CASE("save: up, what the release wants and the file lacked is given")
{
  const auto read = Load(Bytes(Acad() / "empty-r2000.dxf"));
  REQUIRE(read);
  const auto newer = Tests::Written(read->Document, Options{.Release = Version::R2018});
  REQUIRE(newer);
  CHECK(newer->contains("$LASTSAVEDBY"));
  CHECK(newer->contains("$DIMTXTDIRECTION"));
}

TEST_CASE("save: a table's head whole in any release")
{
  const auto read = Load(Bytes(Acad() / "empty-r2018.dxf"));
  REQUIRE(read);
  const auto older = Tests::Written(read->Document, Options{.Release = Version::R2013});
  REQUIRE(older);
  CHECK(older->contains("100\r\nAcDbDimStyleTable\r\n 71\r\n"));
}

TEST_CASE("save: a class's instance count only from R2004")
{
  const auto read = Load(Bytes(Acad() / "empty-r2018.dxf"));
  REQUIRE(read);
  const auto older = Tests::Written(read->Document, Options{.Release = Version::R2000});
  REQUIRE(older);
  const std::string_view head = "  0\r\nCLASS\r\n";
  const std::string_view text = *older;
  const std::string_view one = text.substr(text.find(head), text.find(head, text.find(head) + 1) - text.find(head));
  CHECK_FALSE(one.contains(" 91\r\n"));
  const auto back = Load(Bytes(Acad() / "empty-r2000.dxf"));
  REQUIRE(back);
  const auto newer = Tests::Written(back->Document, Options{.Release = Version::R2018});
  REQUIRE(newer);
  CHECK(newer->contains(" 91\r\n        0\r\n280\r\n"));
}

TEST_CASE("save: ACDSDATA only from R2013")
{
  const auto read = Load(Bytes(Acad() / "empty-r2018.dxf"));
  REQUIRE(read);
  CHECK(Tests::Written(read->Document, Options{.Release = Version::R2013})->contains("ACDSDATA"));
  CHECK_FALSE(Tests::Written(read->Document, Options{.Release = Version::R2010})->contains("ACDSDATA"));
}

TEST_CASE("save: a word holding a line end is escaped, not written raw")
{
  const auto read = Load(Bytes(Fuzz / "found" / "word-with-a-line-end.dxf"));
  REQUIRE(read);
  const auto saved = Tests::Written(read->Document, Options{.Release = Version::R2018});
  REQUIRE(saved);
  CHECK(Load(*saved));
}

TEST_CASE("save: every input the fuzzer found keeps the oracle")
{
  const std::vector<std::filesystem::path> found = Files(Fuzz / "found");
  for (const auto &path : found)
  {
    CAPTURE(path.filename().string());
    CHECK(Broken(Bytes(path)).empty());
  }
}

TEST_CASE("save: text before R2007 in the page asked for, the document's else")
{
  const auto read = Load("  0\nSECTION\n  2\nHEADER\n  9\n$ACADVER\n"
                         "  1\nAC1032\n  0\nENDSEC\n"
                         "  0\nSECTION\n  2\nENTITIES\n  0\nTEXT\n"
                         "  1\n\xd8\xa7\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  const auto latin = Tests::Written(read->Document, Options{.Release = Version::R2004});
  REQUIRE(latin);
  CHECK(latin->find("\\U+0627") != std::string::npos);
  const auto arabic = Tests::Written(read->Document, Options{.Release = Version::R2004, .Codepage = "ANSI_1256"});
  REQUIRE(arabic);
  CHECK(arabic->find('\xc7') != std::string::npos);
  CHECK(arabic->find("$ACADVER\r\n  1\r\nAC1018\r\n") != std::string::npos);
  const auto again = Load(*arabic);
  REQUIRE(again);
  CHECK(std::get<Entities::Text>(again->Document.Entities.front()).Lettering.Value == "\xd8\xa7");
}

TEST_CASE("save: a code page no page is named for is a failure")
{
  const Document drawing = Create(Version::R2000);
  const auto refused = Save(drawing, Options{.Release = Version::R2000, .Codepage = "ANSI_9999"});
  REQUIRE_FALSE(refused);
  CHECK(refused.error().What == "no code page named ANSI_9999");
  CHECK(Save(drawing, Options{.Release = Version::R2000, .Codepage = "ansi_1252"}));
}

TEST_CASE("save: the options keep their own code page name")
{
  const auto read = Load("  0\nSECTION\n  2\nHEADER\n  9\n$ACADVER\n"
                         "  1\nAC1032\n  0\nENDSEC\n"
                         "  0\nSECTION\n  2\nENTITIES\n  0\nTEXT\n"
                         "  1\n\xd8\xa7\n  0\nENDSEC\n  0\nEOF\n");
  REQUIRE(read);
  std::string name = "ANSI_1256";
  Options with{.Release = Version::R2004};
  with.Codepage = name;
  name.replace(0, name.size(), "ANSI_1252");
  const auto arabic = Tests::Written(read->Document, with);
  REQUIRE(arabic);
  CHECK(arabic->find('\xc7') != std::string::npos);
}

TEST_CASE("save: a section that holds something is written where DXF puts it, named in the order or not")
{
  std::string text = Acad("empty-r2000.dxf");
  const std::size_t name = text.find("  2\r\nENTITIES\r\n");
  const std::size_t start = text.rfind("  0\r\nSECTION\r\n", name);
  const std::size_t end = text.find("  0\r\nENDSEC\r\n", name) + 13;
  text.erase(start, end - start);
  auto read = Load(text);
  REQUIRE(read);
  CHECK_FALSE(std::ranges::contains(read->Document.Order, "ENTITIES"));
  REQUIRE(Edit::Add(read->Document, Entities::Line{}));
  const auto saved = Tests::Written(read->Document, {.Release = Version::R2000});
  REQUIRE(saved);
  const std::size_t blocks = saved->find("  2\r\nBLOCKS\r\n");
  const std::size_t entities = saved->find("  2\r\nENTITIES\r\n");
  const std::size_t objects = saved->find("  2\r\nOBJECTS\r\n");
  REQUIRE(entities != std::string::npos);
  CHECK(blocks < entities);
  CHECK(entities < objects);
  CHECK(saved->find("  0\r\nLINE\r\n", entities) < objects);
  CHECK_FALSE(std::ranges::contains(read->Document.Order, "ENTITIES"));
}

TEST_CASE("save: a drawing made by hand writes its sections in DXF's order, and a table its records hold")
{
  Document drawing{};
  drawing.Entities.push_back(Entities::Line{});
  Tables::Layer layer{};
  layer.Record.Name = "WALLS";
  drawing.Layer.Records.push_back(layer);
  drawing.Sections.push_back(Section{.Name = "THUMBNAILIMAGE", .Groups = {}});
  const auto saved = Tests::Written(drawing, {.Release = Version::R12});
  REQUIRE(saved);
  CHECK(*saved ==
        "  0\r\nSECTION\r\n  2\r\nTABLES\r\n  0\r\nTABLE\r\n  2\r\nLAYER\r\n 70\r\n     0\r\n"
        "  0\r\nLAYER\r\n  2\r\nWALLS\r\n 70\r\n     0\r\n 62\r\n     7\r\n  6\r\nContinuous\r\n  0\r\nENDTAB\r\n"
        "  0\r\nENDSEC\r\n"
        "  0\r\nSECTION\r\n  2\r\nENTITIES\r\n  0\r\nLINE\r\n  8\r\n0\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n"
        " 30\r\n0.0\r\n 11\r\n0.0\r\n 21\r\n0.0\r\n 31\r\n0.0\r\n  0\r\nENDSEC\r\n"
        "  0\r\nSECTION\r\n  2\r\nTHUMBNAILIMAGE\r\n  0\r\nENDSEC\r\n  0\r\nEOF\r\n");
}

TEST_CASE("save: a section named in the order but held nowhere stays empty")
{
  auto read = Load(Bytes(Acad() / "empty-r2018.dxf"));
  REQUIRE(read);
  read->Document.Order.emplace_back("NOSUCHSECTION");
  const auto saved = Tests::Written(read->Document, Options{.Release = Version::R2018});
  REQUIRE(saved);
  CHECK(saved->contains("NOSUCHSECTION\r\n  0\r\nENDSEC"));
}

TEST_CASE("save: a variable no release knows stays, in any release")
{
  auto read = Load(Bytes(Acad() / "empty-r2018.dxf"));
  REQUIRE(read);
  read->Document.Header.push_back(Header::Variable{
      .Name = "$NOSUCH",
      .Values = {Group{.Code = 3, .Value = std::int64_t{1}}},
  });
  const auto down = Tests::Written(read->Document, Options{.Release = Version::R2013});
  REQUIRE(down);
  CHECK(down->contains("$NOSUCH"));
  const auto same = Tests::Written(read->Document, Options{.Release = Version::R2018});
  REQUIRE(same);
  CHECK(same->contains("$NOSUCH"));
}

TEST_CASE("save: a code page variable on another group written as it came")
{
  auto read = Load(Bytes(Acad() / "empty-r2018.dxf"));
  REQUIRE(read);
  auto &header = read->Document.Header;
  auto found = std::ranges::find(header, "$DWGCODEPAGE", &Header::Variable::Name);
  REQUIRE(found != header.end());
  found->Values = {Group{.Code = 1, .Value = std::string{"ANSI_1252"}}};
  const auto saved = Tests::Written(read->Document, Options{.Release = Version::R2018});
  REQUIRE(saved);
  CHECK(saved->contains("$DWGCODEPAGE\r\n  1\r\nANSI_1252"));
}

TEST_CASE("save: a value its group cannot hold fails in ASCII as in binary")
{
  Document drawing = Create(Version::R2018);
  drawing.Objects.emplace_back(
      Unknown{.Type = Symbol{"FOO"}, .Base = {}, .Groups = {Group{.Code = 70, .Value = 1e30}}});
  CHECK_FALSE(Save(drawing));
  CHECK_FALSE(Save(drawing, Options{.Binary = true}));
}

TEST_CASE("save: a 16 bit value an ASCII file held above 32767 saves as binary")
{
  std::string text = Bytes(Acad() / "empty-r2018.dxf");
  const std::string_view classes = "  0\r\nSECTION\r\n  2\r\nCLASSES\r\n";
  text.insert(text.find(classes), "  0\r\nSECTION\r\n  2\r\nACME\r\n1070\r\n40000\r\n  0\r\nENDSEC\r\n");
  const auto read = Load(text);
  REQUIRE(read);
  CHECK(Save(read->Document, Options{.Release = Version::R2018, .Binary = true}));
}

TEST_CASE("save: binary, every drawing AutoCAD wrote reads back the same")
{
  for (const auto &path : Files(Acad()))
  {
    CAPTURE(path.filename().string());
    const auto read = Load(Bytes(path));
    REQUIRE(read);
    const Options with{.Release = Header::Declared(read->Document.Header), .Binary = true};
    const auto bytes = Tests::Written(read->Document, with);
    REQUIRE(bytes);
    CHECK(bytes->starts_with(Tags::Sentinel()));
    const auto again = Load(*bytes);
    REQUIRE(again);
    CHECK(again->Document == read->Document);
  }
}

TEST_CASE("save: binary, a new drawing in every release")
{
  for (const Version release : {Version::R10, Version::R12, Version::R13, Version::R14, Version::R2000, Version::R2018})
  {
    CAPTURE(release);
    const Document drawing = Create(release);
    const auto bytes = Tests::Written(drawing, Options{.Release = release, .Binary = true});
    REQUIRE(bytes);
    const auto read = Load(*bytes);
    REQUIRE(read);
    CHECK(Tests::Written(read->Document, Options{.Release = release}) ==
          Tests::Written(drawing, Options{.Release = release}));
  }
}
} // namespace DxfIO

namespace DxfIO
{
TEST_CASE("save: a code page given by another group is written by its own")
{
  constexpr std::string_view page = "$DWGCODEPAGE\r\n  3\r\n";
  std::string text = Bytes(Acad() / "texts-r2000.dxf");
  text.replace(text.find(page), page.size(), "$DWGCODEPAGE\r\n  1\r\n");
  const auto read = Load(text);
  REQUIRE(read);
  const auto same = Tests::Written(read->Document, Options{.Release = Version::R2000});
  REQUIRE(same);
  CHECK_FALSE(same->contains("$DWGCODEPAGE\r\n  1\r\n"));
  CHECK(same->contains("$DWGCODEPAGE\r\n  3\r\n"));
}

TEST_CASE("save: a variable with no value, or not as many, takes the release's own")
{
  Document drawing = Create(Version::R2018);
  for (Header::Variable &one : drawing.Header)
  {
    if (one.Name == "$INSUNITS")
    {
      one.Values.clear();
    }
    if (one.Name == "$EXTMIN")
    {
      one.Values = {Group{.Code = 10, .Value = 1.0}};
    }
    if (one.Name == "$DWGCODEPAGE")
    {
      one.Values = {Group{.Code = 3, .Value = std::int64_t{5}}};
    }
  }
  const auto older = Tests::Written(drawing, Options{.Release = Version::R2000});
  REQUIRE(older);
  CHECK(older->contains("$EXTMIN\r\n 10\r\n"));
  CHECK_FALSE(older->contains("$EXTMIN\r\n 10\r\n1.0\r\n  9"));
}

TEST_CASE("save: what a release cannot hold is noted beside the bytes")
{
  Document drawing = Create(Version::R2018);
  drawing.Entities.emplace_back(Entities::MultiLeader{});
  const auto saved = Save(drawing, Options{.Release = Version::R12});
  REQUIRE(saved);
  CHECK_FALSE(saved->Bytes.empty());
  CHECK_FALSE(saved->Issues.empty());
  const auto same = Save(drawing);
  REQUIRE(same);
  CHECK(same->Issues.empty());
}

TEST_CASE("save: a drawing writes to a stream and to a file")
{
  const Document drawing = Create(Version::R2018);
  const auto whole = Save(drawing);
  REQUIRE(whole);
  std::ostringstream to;
  const auto streamed = Save(drawing, to, Options{});
  REQUIRE(streamed);
  CHECK(streamed->empty());
  CHECK(to.str() == whole->Bytes);
  const std::filesystem::path file = std::filesystem::temp_directory_path() / "dxfio-save-test.dxf";
  const auto filed = SaveFile(drawing, file);
  REQUIRE(filed);
  CHECK(Bytes(file) == whole->Bytes);
  std::filesystem::remove(file);
}

TEST_CASE("save: a drawing that cannot be written, a stream that fails or a file out of reach is told")
{
  Document drawing = Create(Version::R2018);
  Entities::Line line{};
  line.Start.X = std::numeric_limits<double>::quiet_NaN();
  drawing.Entities.emplace_back(line);
  CHECK_FALSE(Save(drawing));
  std::ostringstream unused;
  CHECK_FALSE(Save(drawing, unused));
  CHECK(unused.str().empty());

  const Document fresh = Create(Version::R2018);
  std::ostringstream broken;
  broken.setstate(std::ios::badbit);
  const auto refused = Save(fresh, broken);
  REQUIRE_FALSE(refused);
  CHECK(refused.error().What == "the stream would not take the drawing");
  const std::filesystem::path nowhere = std::filesystem::temp_directory_path() / "dxfio-missing" / "out.dxf";
  const auto unwritten = SaveFile(fresh, nowhere);
  REQUIRE_FALSE(unwritten);
  CHECK(unwritten.error().What == nowhere.string() + " cannot be written");
}

namespace
{
class Unsynced : public std::stringbuf
{
protected:
  auto sync() -> int override { return -1; }
};
} // namespace

TEST_CASE("save: a stream that takes the bytes but cannot flush them is told")
{
  Unsynced full;
  std::ostream to{&full};
  const auto refused = Save(Create(Version::R2018), to);
  REQUIRE_FALSE(refused);
  CHECK(refused.error().What == "the stream would not take the drawing");
}
} // namespace DxfIO
