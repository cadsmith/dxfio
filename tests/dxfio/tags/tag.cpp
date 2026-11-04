#include "tags/tag.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <string_view>

namespace DxfIO::Tags
{
TEST_CASE("tag: two tags are equal by their code and value, not their line")
{
  CHECK(Tag{.Code = 40, .Value = 1.5, .Line = 7} == Tag{.Code = 40, .Value = 1.5, .Line = 9});
  CHECK_FALSE(Tag{.Code = 40, .Value = 1.5} == Tag{.Code = 41, .Value = 1.5});
  CHECK_FALSE(Tag{.Code = 70, .Value = std::int64_t{1}} == Tag{.Code = 70, .Value = true});
}

TEST_CASE("tag: two chunks are equal by their bytes")
{
  constexpr std::string_view one{"\x00\xff\x10", 3};
  constexpr std::string_view other{"\x00\xff\x10", 3};
  constexpr std::string_view shorter{"\x00\xff", 2};
  CHECK(Chunk{one} == Chunk{other});
  CHECK_FALSE(Chunk{one} == Chunk{shorter});
  CHECK_FALSE(Tag{.Code = 310, .Value = Chunk{one}} == Tag{.Code = 310, .Value = one});
}

TEST_CASE("tag: the text a tag holds, none when it holds a number")
{
  CHECK(Text(Tag{.Code = 0, .Value = std::string_view{"LINE"}}) == "LINE");
  CHECK(Text(Tag{.Code = 70, .Value = std::int64_t{1}}).empty());
}

TEST_CASE("tag: a tag holds a text under a code, not a number")
{
  const Tag line{.Code = 0, .Value = std::string_view{"LINE"}};
  CHECK(Holds(line, 0, "LINE"));
  CHECK_FALSE(Holds(line, 0, "CIRCLE"));
  CHECK_FALSE(Holds(line, 8, "LINE"));
  CHECK_FALSE(Holds(Tag{.Code = 0, .Value = Chunk{}}, 0, ""));
}
} // namespace DxfIO::Tags
