#include "objects/shapes/data.hpp"

#include "../alike.hpp"
#include "../entities/drawn.hpp"
#include "../kind.hpp"
#include "objects/io.hpp"

#include <string>
#include <variant>
#include <vector>

namespace DxfIO::Objects
{
namespace
{
auto Taken() -> std::string
{
  return Entities::Drawn("421", Version::R2004, "schedules");
}

auto Made() -> DataTable
{
  DataTable out{};
  out.Base.Self = Handle{.Value = 0x2a};
  out.Rows = 2;
  out.Name = "Parts";
  out.Columns = {
      Column{.Kind = 1,
             .Name = "Count",
             .Cells = {DxfIO::Group{.Code = 93, .Value = std::int64_t{3}},
                       DxfIO::Group{.Code = 93, .Value = std::int64_t{4}}}},
      Column{.Kind = 4,
             .Name = "Part",
             .Cells = {DxfIO::Group{.Code = 3, .Value = std::string{"bolt"}},
                       DxfIO::Group{.Code = 3, .Value = std::string{"nut"}}}},
  };
  return out;
}
} // namespace

TEST_CASE("data table: AutoCAD's own, a column each of its kinds")
{
  const std::string text = Taken();
  const DataTable one = DxfIO::Read<DataTable>(text, Version::R2004);
  CHECK(one.Base.Extra.empty());
  CHECK(Alike(Written(one, Version::R2004), text));
  CHECK(one.Version == 2);
  CHECK(one.Rows == 8);
  REQUIRE(one.Columns.size() == 4);
  CHECK(one.Columns.front().Kind == 2);
  CHECK(one.Columns.front().Cells.size() == 8);
  CHECK(one.Columns.at(1).Cells.front().Code == 93);
  CHECK(one.Columns.back().Kind == 6);
  CHECK(one.Columns.back().Cells.front().Code == 360);
}

TEST_CASE("data table: one made by hand reads back the same")
{
  const DataTable one = Made();
  CHECK(DxfIO::Read<DataTable>(Written(one)) == one);
}

TEST_CASE("data table: empty, no column at all")
{
  DataTable one{};
  one.Base.Self = Handle{.Value = 0x2a};
  CHECK(DxfIO::Read<DataTable>(Written(one)) == one);
}

TEST_CASE("data table: a cell before any column stays as it came")
{
  std::string text = Written(Made());
  const std::string from = " 92\r\n";
  text.insert(text.find(from), " 93\r\n        1\r\n");
  const auto all = Tags::All(text);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  const std::vector<Object> kept = Objects::Read(records, Fields::Reading{.Text = Encoding::Utf8()});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Unknown>(kept.front()));
}

TEST_CASE("data table: a column type past an int stays as it came")
{
  std::string text = Written(Made());
  const std::string from = " 92\r\n";
  const std::size_t at = text.find(from) + from.size();
  text.replace(at, text.find("\r\n", at) - at, "4294967296");
  const auto all = Tags::All(text);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  const std::vector<Object> kept = Objects::Read(records, Fields::Reading{.Text = Encoding::Utf8()});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Unknown>(kept.front()));
}

TEST_CASE("data table: R2000 names it and its subclass in capitals")
{
  const std::string text = Entities::Drawn("492", Version::R2000, "schedules");
  const AcDbDataTable one = DxfIO::Read<AcDbDataTable>(text, Version::R2000);
  CHECK(one.Base.Extra.empty());
  CHECK(Alike(Written(one, Version::R2000), text));
  CHECK(one.Columns.size() == 4);
  CHECK(text.find("100\r\nACDBDATATABLE\r\n") != std::string::npos);
}

TEST_CASE("data table: named as DXF names it")
{
  CHECK(Objects::Type(Object{DataTable{}}) == "DATATABLE");
  CHECK(Objects::Type(Object{AcDbDataTable{}}) == "ACDBDATATABLE");
}
} // namespace DxfIO::Objects

namespace DxfIO::Objects
{
TEST_CASE("data table: a type with no name after it is a cell, last or not")
{
  const auto kept = Columned(
      {DxfIO::Group{.Code = 92, .Value = std::int64_t{1}}, DxfIO::Group{.Code = 2, .Value = std::string{"Count"}},
       DxfIO::Group{.Code = 92, .Value = std::int64_t{4}}, DxfIO::Group{.Code = 93, .Value = std::int64_t{3}},
       DxfIO::Group{.Code = 92, .Value = std::int64_t{5}}});
  REQUIRE(kept);
  REQUIRE(kept->size() == 1);
  CHECK(kept->front().Cells.size() == 3);
}

TEST_CASE("data table: a type no int holds opens no column")
{
  CHECK_FALSE(Columned({DxfIO::Group{.Code = 92, .Value = std::int64_t{4294967296}},
                        DxfIO::Group{.Code = 2, .Value = std::string{"Count"}}}));
  CHECK_FALSE(Columned({DxfIO::Group{.Code = 92, .Value = std::int64_t{-2147483649}},
                        DxfIO::Group{.Code = 2, .Value = std::string{"Count"}}}));
}
} // namespace DxfIO::Objects
