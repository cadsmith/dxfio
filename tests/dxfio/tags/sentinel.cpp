#include "tags/sentinel.hpp"

#include <doctest/doctest.h>

#include <string>
#include <string_view>

namespace DxfIO::Tags
{
TEST_CASE("sentinel: a binary file starts with it, its NUL included")
{
  CHECK(Sentinel().size() == 22);
  CHECK(Sentinel().ends_with(std::string_view{"\x1a\0", 2}));
  CHECK(Marked(Sentinel()));
  CHECK(Marked(std::string{Sentinel()} + "SECTION"));
  CHECK_FALSE(Marked("AutoCAD Binary DXF\r\n\x1a"));
  CHECK_FALSE(Marked("  0\nSECTION\n"));
  CHECK_FALSE(Marked(""));
}
} // namespace DxfIO::Tags
