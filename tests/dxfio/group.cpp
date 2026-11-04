#include "dxfio/group.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <string>

namespace DxfIO
{
TEST_CASE("group: compared by its code and what it holds")
{
  CHECK(Group{.Code = 1000, .Value = std::string{"walls"}} == Group{.Code = 1000, .Value = std::string{"walls"}});
  CHECK_FALSE(Group{.Code = 1070, .Value = std::int64_t{1}} == Group{.Code = 1071, .Value = std::int64_t{1}});
  CHECK_FALSE(Group{.Code = 1005, .Value = Handle{1}} == Group{.Code = 1005, .Value = std::int64_t{1}});
  CHECK(Group{.Code = 1004, .Value = Chunk{std::string{"\x01\x02"}}} ==
        Group{.Code = 1004, .Value = Chunk{std::string{"\x01\x02"}}});
}
} // namespace DxfIO
