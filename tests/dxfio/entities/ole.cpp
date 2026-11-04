#include "entities/shapes/ole.hpp"

#include "../kind.hpp"
#include "entities/io.hpp"

#include <string>
#include <variant>
#include <vector>

namespace DxfIO::Entities
{
namespace
{
const std::string Embedding = "  0\r\nOLE2FRAME\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
                              "  8\r\n0\r\n100\r\nAcDbOle2Frame\r\n 70\r\n     2\r\n"
                              "  3\r\nPaintbrush Picture\r\n"
                              " 10\r\n4.0\r\n 20\r\n5.0\r\n 30\r\n0.0\r\n"
                              " 11\r\n6.0\r\n 21\r\n1.0\r\n 31\r\n0.0\r\n"
                              " 71\r\n     2\r\n 72\r\n     0\r\n 90\r\n        4\r\n"
                              "310\r\nDEADBEEF\r\n  1\r\nOLE\r\n";
} // namespace

TEST_CASE("ole: an embedded object, its corners and its bytes")
{
  const Ole2Frame one = Replayed<Ole2Frame>(Embedding);
  CHECK(one.Frame->Version == 2);
  CHECK(one.Frame->Description == "Paintbrush Picture");
  CHECK(one.Frame->Upper == Vector{.X = 4, .Y = 5});
  CHECK(one.Frame->Lower == Vector{.X = 6, .Y = 1});
  CHECK(one.Frame->Kind == Embedded);
  CHECK(one.Frame->Space == 0);
  CHECK(one.Frame->Data == std::string{"\xDE\xAD\xBE\xEF"});
}

TEST_CASE("ole: bytes past one line split as AutoCAD splits them")
{
  Ole2Frame one{};
  one.Record.Base.Self = Handle{.Value = 0x2a};
  one.Frame->Data = std::string(300, '\x7f');
  const std::string text = Written(one);
  CHECK(text.find(" 90\r\n      300\r\n") != std::string::npos);
  const Ole2Frame back = DxfIO::Read<Ole2Frame>(text);
  CHECK(back == one);
}

TEST_CASE("ole: no bytes, none written")
{
  const std::string text = Written(Ole2Frame{});
  CHECK(text.find("310\r\n") == std::string::npos);
  CHECK(text.ends_with("  1\r\nOLE\r\n"));
  CHECK(DxfIO::Read<Ole2Frame>(text).Frame->Data.empty());
}

TEST_CASE("ole: without its end it stays as it came")
{
  const std::string cut = Embedding.substr(0, Embedding.find("  1\r\nOLE")) + " 77\r\n     1\r\n";
  const auto all = Tags::All(cut);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8()});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Unknown>(kept.front()));
}

TEST_CASE("ole: named as DXF names it")
{
  CHECK(Named(Entity{Ole2Frame{}}) == "OLE2FRAME");
}
} // namespace DxfIO::Entities

namespace DxfIO::Entities
{
TEST_CASE("ole: bytes not in hex keep the record as it came")
{
  std::string bad = Embedding;
  bad.replace(bad.find("DEADBEEF"), 8, "ZZ");
  const auto all = Tags::All(bad);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8()});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Unknown>(kept.front()));
}
} // namespace DxfIO::Entities
