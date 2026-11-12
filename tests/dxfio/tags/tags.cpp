#include "tags/tags.hpp"

#include "tags/sentinel.hpp"

#include <doctest/doctest.h>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

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
    REQUIRE(tags->size()   == 2);
    CHECK(tags->at(0)      == Tag{.Code = 0, .Value = std::string_view{"SECTION"}});
    CHECK(tags->at(1).Line == 7);
  }

  TEST_CASE("tags: a binary file is read after its sentinel")
  {
    std::string bytes{Sentinel()};
    bytes.append(std::string_view{"\x00\x00SECTION\x00\x02\x00HEADER\x00", 19});
    const auto tags = All(bytes);
    REQUIRE(tags);
    REQUIRE(tags->size() == 2);
    CHECK(tags->at(1)    == Tag{.Code = 2, .Value = std::string_view{"HEADER"}});
  }

  TEST_CASE("tags: what a reader leaves out is told to the issues asked for")
  {
    std::vector<Issue> issues;
    const auto         tags = All("  0\nSECTION\n 40\nnope\n  2\nHEADER\n", &issues);
    REQUIRE(tags);
    REQUIRE(tags->size()   == 2);
    CHECK(tags->at(1).Line == 5);
    CHECK(issues
          == std::vector{
              Issue{.Line = 4, .What = "number expected for group 40, found \"nope\", left out"}});
    CHECK(All("  0\nSECTION\n 40\nnope\n")->size() == 1);
  }

  TEST_CASE("tags: reading stops at EOF, and what follows it is told")
  {
    std::vector<Issue> issues;
    const auto         tags = All("  0\nSECTION\n  0\nEOF\ngarbage\n  0\nSECTION\n", &issues);
    REQUIRE(tags);
    CHECK(tags->size() == 2);
    CHECK(issues       == std::vector{Issue{.Line = 5, .What = "what follows EOF, left out"}});
    CHECK(All("  0\nEOF\ngarbage\n")->size() == 1);
    std::vector<Issue> none;
    CHECK(All("  0\nEOF\n\n   \n", &none)->size() == 1);
    CHECK(none.empty());
    std::string bytes{Sentinel()};
    bytes.append(std::string_view{"\x00\x00"
                                  "EOF\x00\x00\x00"
                                  "SECTION\x00",
                                  16});
    std::vector<Issue> binary;
    CHECK(All(bytes, &binary)->size() == 1);
    CHECK(binary == std::vector{Issue{.Line = 0, .What = "what follows EOF, left out"}});
  }

  TEST_CASE("tags: the failure that stops the file is handed back")
  {
    CHECK(All("  0\nSECTION\nabc\n")
          == std::unexpected{Failure{3, "group code expected, found \"abc\""}});
  }
} // namespace DxfIO::Tags
