#include "fields/owned.hpp"

#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace DxfIO::Fields
{
TEST_CASE("owned: a tag as a group, its text decoded")
{
  const Encoding::Scheme latin = Encoding::Paged(Encoding::Standard());
  const Tags::Tag text{.Code = 1000, .Value = std::string_view{"caf\xe9"}};
  CHECK(Owned(text, latin) == Group{.Code = 1000, .Value = std::string{"caf\xc3\xa9"}});
  CHECK(Owned(Tags::Tag{.Code = 1070, .Value = std::int64_t{3}}, latin) ==
        Group{.Code = 1070, .Value = std::int64_t{3}});
}

TEST_CASE("owned: a handle's hex a handle, a chunk's hex its bytes")
{
  const Encoding::Scheme utf8 = Encoding::Utf8();
  CHECK(Owned(Tags::Tag{.Code = 330, .Value = std::string_view{"1F"}}, utf8) ==
        Group{.Code = 330, .Value = Handle{0x1f}});
  CHECK(Owned(Tags::Tag{.Code = 5, .Value = std::string_view{"ARROW"}}, utf8) ==
        Group{.Code = 5, .Value = std::string{"ARROW"}});
  CHECK(Owned(Tags::Tag{.Code = 310, .Value = std::string_view{"DEAD01"}}, utf8) ==
        Group{.Code = 310, .Value = Chunk{std::string{"\xde\xad\x01"}}});
}

TEST_CASE("owned: a word of the format kept as it is, escapes and all")
{
  const Encoding::Scheme utf8 = Encoding::Utf8();
  for (const int code : {0, 5, 100, 102, 310, 330})
  {
    const std::string word{"0\\U+0064"};
    const Tags::Tag one{.Code = code, .Value = std::string_view{word}};
    const Group kept = Owned(one, utf8);
    CHECK(kept == Group{.Code = code, .Value = word});
    std::string out;
    Tags::Writer writer{out};
    Writing to{.To = &writer, .Release = Version::R2018, .Text = utf8, .From = Version::R2018};
    REQUIRE(Put(to, kept));
    CHECK(out.contains(word));
  }
}

TEST_CASE("owned: a group written back as the tag it came from")
{
  const std::string_view text = "1000\r\ncaf\xe9\r\n330\r\n1F\r\n"
                                "310\r\nDEAD01\r\n1070\r\n     3\r\n";
  const auto all = Tags::All(text);
  REQUIRE(all);
  std::string out;
  Tags::Writer writer{out};
  Writing to{.To = &writer, .Release = Version::R2004, .Text = Encoding::Paged(Encoding::Standard())};
  for (const Tags::Tag &one : *all)
  {
    REQUIRE(Put(to, Owned(one, to.Text)));
  }
  CHECK(out == text);
}

TEST_CASE("owned: a chunk of an odd count of digits stays text")
{
  CHECK(Owned(Tags::Tag{.Code = 310, .Value = std::string_view{"ABC"}}, Encoding::Utf8()) ==
        Group{.Code = 310, .Value = std::string{"ABC"}});
}
} // namespace DxfIO::Fields
