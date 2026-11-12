#include "fields/picture.hpp"

#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
  namespace
  {
    struct Shown
    {
      std::string Bytes;
    };

    const std::tuple Shapes{Picture{.Member = &Shown::Bytes}};

    auto Read(std::string_view text) -> std::string
    {
      const auto all = Tags::All(text);
      REQUIRE(all);
      Walk  in{*all, Reading{}};
      Shown out{};
      Fields::Read(out, in, Shapes);
      CHECK(in.Rest().empty());
      return out.Bytes;
    }
  } // namespace

  TEST_CASE("picture: its chunks joined, whichever code gave the size")
  {
    CHECK(Read(" 92\n3\n310\nAB\n310\nCDEF\n") == "\xab\xcd\xef");
    CHECK(Read("160\n3\n310\nABCDEF\n")        == "\xab\xcd\xef");
    CHECK(Read("").empty());
  }

  TEST_CASE("picture: its size on the release's code, then its bytes")
  {
    const std::string bytes = "\xab\xcd";
    Sheet             old{Version::R2007};
    REQUIRE(Write(Shown{.Bytes = bytes}, old.To, Shapes));
    CHECK(old.Out.starts_with(" 92\r\n"));
    CHECK(old.Out.contains("310\r\nABCD\r\n"));
    Sheet modern{Version::R2010};
    REQUIRE(Write(Shown{.Bytes = bytes}, modern.To, Shapes));
    CHECK(modern.Out.starts_with("160\r\n"));
  }

  TEST_CASE("picture: none written for none or for another release")
  {
    Sheet empty;
    REQUIRE(Write(Shown{}, empty.To, Shapes));
    CHECK(empty.Out.empty());
    Sheet other{Version::R2007};
    other.To.From = Version::R2018;
    REQUIRE(Write(Shown{.Bytes = "x"}, other.To, Shapes));
    CHECK(other.Out.empty());
  }
} // namespace DxfIO::Fields

namespace DxfIO::Fields
{
  TEST_CASE("picture: a chunk not in hex refuses the record")
  {
    const std::vector<Tags::Tag> all{
        Tags::Tag{.Code = 160, .Value = std::int64_t{1}},
        Tags::Tag{.Code = 310, .Value = std::string_view{"ZZ"}},
    };
    Walk  in{all, Reading{}};
    Shown out{};
    Fields::Read(out, in, Shapes);
    CHECK(out.Bytes.empty());
    CHECK_FALSE(in.Whole());
  }

  TEST_CASE("picture: a chunk in hex only in part adds none of its bytes")
  {
    const std::vector<Tags::Tag> all{
        Tags::Tag{.Code = 160, .Value = std::int64_t{4}},
        Tags::Tag{.Code = 310, .Value = std::string_view{"ABCD"}},
        Tags::Tag{.Code = 310, .Value = std::string_view{"EFZZ"}},
    };
    Walk  in{all, Reading{}};
    Shown out{};
    Fields::Read(out, in, Shapes);
    CHECK(out.Bytes == "\xab\xcd");
    CHECK_FALSE(in.Whole());
  }

  TEST_CASE("picture: the chunks of a binary file joined as they are")
  {
    const std::vector<Tags::Tag> all{
        Tags::Tag{.Code = 160, .Value = std::int64_t{3}},
        Tags::Tag{.Code = 310, .Value = Tags::Chunk{.Bytes = "\xab"}},
        Tags::Tag{.Code = 310, .Value = Tags::Chunk{.Bytes = "\xcd\xef"}},
    };
    Walk  in{all, Reading{}};
    Shown out{};
    Fields::Read(out, in, Shapes);
    CHECK(out.Bytes == "\xab\xcd\xef");
    CHECK(in.Whole());
  }
} // namespace DxfIO::Fields
