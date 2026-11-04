#include "tags/ascii/reader.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Tags::Ascii
{
namespace
{
auto Read(std::string_view text, std::vector<Issue> *issues = nullptr) -> std::vector<Tag>
{
  Reader in{text, issues};
  std::vector<Tag> out;
  while (!in.Done())
  {
    const std::expected<std::optional<Tag>, Failure> one = in.Next();
    REQUIRE(one);
    if (*one)
    {
      out.push_back(**one);
    }
  }
  return out;
}

auto Refused(std::string_view text) -> Failure
{
  Reader in{text, nullptr};
  while (!in.Done())
  {
    const std::expected<std::optional<Tag>, Failure> one = in.Next();
    if (!one)
    {
      return one.error();
    }
  }
  FAIL("the text was read whole");
  return {};
}

auto Told(std::string_view text) -> std::vector<Issue>
{
  std::vector<Issue> issues;
  Read(text, &issues);
  return issues;
}
} // namespace

TEST_CASE("ascii: codes padded, either line ending and a byte order mark")
{
  const std::vector<Tag> tags = Read("\xef\xbb\xbf  0\r\nSECTION\r\n  2\r\nHEADER\r\n"
                                     "  9\n$ACADVER\n  1\nAC1027\n"
                                     " 70\r\n   104\r\n 40\r\n-0.5\r\n290\r\n     1\r\n"
                                     "  1\r\n\r\n999\r\n  note \r\n  0\r\nEOF\r\n");
  REQUIRE(tags.size() == 10);
  CHECK(tags.at(0) == Tag{.Code = 0, .Value = std::string_view{"SECTION"}});
  CHECK(tags.at(0).Line == 1);
  CHECK(tags.at(3) == Tag{.Code = 1, .Value = std::string_view{"AC1027"}});
  CHECK(tags.at(4) == Tag{.Code = 70, .Value = std::int64_t{104}});
  CHECK(tags.at(5) == Tag{.Code = 40, .Value = -0.5});
  CHECK(tags.at(6) == Tag{.Code = 290, .Value = true});
  CHECK(tags.at(7) == Tag{.Code = 1, .Value = std::string_view{}});
  CHECK(tags.at(8) == Tag{.Code = 999, .Value = std::string_view{"  note "}});
  CHECK(tags.at(9).Line == 19);
}

TEST_CASE("ascii: a lone carriage return ends a line, a CRLF ends one")
{
  const std::vector<Tag> tags = Read("  0\rSECTION\r 40\r1.5\r  0\r\nEOF\r\n");
  REQUIRE(tags.size() == 3);
  CHECK(tags.at(1) == Tag{.Code = 40, .Value = 1.5});
  CHECK(tags.at(2).Line == 5);
}

TEST_CASE("ascii: blank lines after the last tag end the file")
{
  CHECK(Read("  0\nEOF\n\n   \r\n\t\n").size() == 1);
  CHECK(Read("").empty());
}

TEST_CASE("ascii: a value that does not parse is left out and told, the rest read on")
{
  std::vector<Issue> issues;
  const std::vector<Tag> tags = Read("  0\nLINE\n 40\nnope\n 70\n1e300\n 62\n     1\n", &issues);
  REQUIRE(tags.size() == 2);
  CHECK(tags.at(1) == Tag{.Code = 62, .Value = std::int64_t{1}, .Line = 7});
  CHECK(issues == std::vector{
                      Issue{.Line = 4, .What = "number expected for group 40, found \"nope\", left out"},
                      Issue{.Line = 6, .What = "integer expected for group 70, found \"1e300\", left out"},
                  });
  CHECK(Read(" 40\nnope\n").empty());
  CHECK(Told(" 70\n100000\n 90\n3000000000\n") ==
        std::vector{
            Issue{.Line = 2, .What = "integer of 16 bits expected for group 70, found \"100000\", left out"},
            Issue{.Line = 4, .What = "integer of 32 bits expected for group 90, found \"3000000000\", left out"},
        });
}

TEST_CASE("ascii: a failure names its line and what was found")
{
  CHECK(Refused("  0\r\nSECTION\r\nabc\r\nx\r\n") == Failure{3, "group code expected, found \"abc\""});
  CHECK(Refused("  0\nSECTION\n1072\nx\n") == Failure{3, "group code expected, found \"1072\""});
  CHECK(Refused("  0\nSECTION\n -1\nx\n") == Failure{3, "group code expected, found \"-1\""});
  CHECK(Refused("  0\nSECTION\n  2") == Failure{3, "value missing after the last group code"});
  CHECK(Refused("  0\nSECTION\n\n  2\nHEADER\n") == Failure{3, "blank line where a group code is expected"});
}

TEST_CASE("ascii: a failure shows no more than the start of a long line")
{
  const std::string line(100000, 'x');
  const std::string shown = std::string(64, 'x') + "...";
  CHECK(Refused(line + "\n  0\n") == Failure{1, "group code expected, found \"" + shown + "\""});
  CHECK(Told(" 40\n" + line + "\n") ==
        std::vector{Issue{.Line = 2, .What = "number expected for group 40, found \"" + shown + "\", left out"}});
  const std::string exact(64, 'y');
  CHECK(Refused(exact + "\n") == Failure{1, "group code expected, found \"" + exact + "\""});
}
} // namespace DxfIO::Tags::Ascii
