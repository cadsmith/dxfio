#include "dxfio/brace.hpp"

#include <doctest/doctest.h>

#include <string>

namespace DxfIO
{
TEST_CASE("brace: a name and the groups between its braces")
{
  const Brace blkrefs{.Name = "BLKREFS", .Groups = {Group{.Code = 331, .Value = Handle{0x2a}}}};
  CHECK(blkrefs == blkrefs);
  CHECK_FALSE(blkrefs == Brace{.Name = "BLKREFS", .Groups = {}});
}
} // namespace DxfIO
