#include "tags/tags.hpp"

#include "tags/sentinel.hpp"

#include <doctest/doctest.h>

#include <string>
#include <string_view>
#include <utility>

namespace DxfIO::Tags
{
namespace
{
template <class BufferT>
constexpr bool Takes = requires(BufferT buffer) { All(std::move(buffer)); };

static_assert(Takes<std::string_view>);
static_assert(Takes<const std::string &>);
static_assert(!Takes<std::string>);
} // namespace

TEST_CASE("tags: an ASCII file is read whole, its comments left out")
{
  const auto tags = All("999\nmade by hand\n  0\nSECTION\n999\nagain\n"
                        "  2\nHEADER\n");
  REQUIRE(tags);
  REQUIRE(tags->size() == 2);
  CHECK(tags->at(0) == Tag{.Code = 0, .Value = std::string_view{"SECTION"}});
  CHECK(tags->at(1).Line == 7);
}

TEST_CASE("tags: a binary file is read after its sentinel")
{
  std::string bytes{Sentinel()};
  bytes.append(std::string_view{"\x00\x00SECTION\x00\x02\x00HEADER\x00", 19});
  const auto tags = All(bytes);
  REQUIRE(tags);
  REQUIRE(tags->size() == 2);
  CHECK(tags->at(1) == Tag{.Code = 2, .Value = std::string_view{"HEADER"}});
}

TEST_CASE("tags: the failure that stops the file is handed back")
{
  CHECK(All("  0\nSECTION\nabc\n") == std::unexpected{Failure{3, "group code expected, found \"abc\""}});
}
} // namespace DxfIO::Tags
