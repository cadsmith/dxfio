#include "dxfio/tag.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <string>

namespace DxfIO
{
  TEST_CASE("group: compared by its code and what it holds")
  {
    CHECK(Tag{.Code = 1000, .Value = std::string{"walls"}}
          == Tag{.Code = 1000, .Value = std::string{"walls"}});
    CHECK_FALSE(Tag{.Code = 1070, .Value = std::int64_t{1}}
                == Tag{.Code = 1071, .Value = std::int64_t{1}});
    CHECK_FALSE(Tag{.Code = 1005, .Value = Handle{1}}
                == Tag{.Code = 1005, .Value = std::int64_t{1}});
    CHECK(Tag{.Code = 1004, .Value = Chunk{std::string{"\x01\x02"}}}
          == Tag{.Code = 1004, .Value = Chunk{std::string{"\x01\x02"}}});
  }
} // namespace DxfIO
