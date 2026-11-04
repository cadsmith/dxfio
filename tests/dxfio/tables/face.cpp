#include "dxfio/tables/face.hpp"

#include "dxfio/group.hpp"
#include "dxfio/xdata.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace DxfIO::Tables
{
namespace
{
auto Held(std::string application, std::vector<Group> groups) -> Style
{
  Style one{};
  one.Record.Base.Extended.push_back(Xdata{.Application = std::move(application), .Groups = std::move(groups)});
  return one;
}
} // namespace

TEST_CASE("face: the family and the flags of ACAD's extended data")
{
  const Style bold = Held("ACAD", {Group{.Code = 1000, .Value = std::string{"Arial"}},
                                   Group{.Code = 1071, .Value = std::int64_t{33554466}}});
  const std::optional<Face> found = Named(bold);
  REQUIRE(found);
  CHECK(found->Family == "Arial");
  CHECK((found->Flags & (1 << 25)) != 0);
  CHECK((found->Flags & (1 << 24)) == 0);
}

TEST_CASE("face: flags of 0 where it gives none")
{
  const std::optional<Face> found = Named(Held("ACAD", {Group{.Code = 1000, .Value = std::string{"Arial"}}}));
  REQUIRE(found);
  CHECK(found->Flags == 0);
}

TEST_CASE("face: none with no family, or from another application")
{
  CHECK_FALSE(Named(Style{}));
  CHECK_FALSE(Named(Held("ACAD", {Group{.Code = 1071, .Value = std::int64_t{34}}})));
  CHECK_FALSE(Named(Held("OTHER", {Group{.Code = 1000, .Value = std::string{"Arial"}}})));
}
} // namespace DxfIO::Tables
