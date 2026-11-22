#include "dxfio/handle.hpp"

#include <doctest/doctest.h>

#include <functional>

namespace DxfIO
{
  TEST_CASE("handle: written in upper case hex")
  {
    CHECK(HexOf(Handle{0x2c20a}) == "2C20A");
    CHECK(HexOf(Handle{})        == "0");
  }

  TEST_CASE("handle: read from hex of either case, blanks aside")
  {
    CHECK(ParseHandle("1F")     == Handle{0x1f});
    CHECK(ParseHandle("1f")     == Handle{0x1f});
    CHECK(ParseHandle(" 2C20A") == Handle{0x2c20a});
    CHECK(ParseHandle("FFFFFFFFFFFFFFFF") == Handle{0xffffffffffffffff});
    CHECK(ParseHandle("0") == Handle{});
  }

  TEST_CASE("handle: what is no hex of sixteen digits at most is none")
  {
    CHECK_FALSE(ParseHandle(""));
    CHECK_FALSE(ParseHandle("ARROW"));
    CHECK_FALSE(ParseHandle("1G"));
    CHECK_FALSE(ParseHandle("10000000000000000"));
  }

  TEST_CASE("handle: 0 names none, handles order and hash by value")
  {
    CHECK_FALSE(Handle{});
    CHECK(Handle{1});
    CHECK(Handle{1} < Handle{2});
    CHECK(std::hash<Handle>{}(Handle{7}) == std::hash<Handle>{}(Handle{7}));
  }
} // namespace DxfIO
