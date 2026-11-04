#include "tags/binary/reader.hpp"

#include <doctest/doctest.h>

#include <array>
#include <bit>
#include <cstdint>
#include <format>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Tags::Binary
{
namespace
{
template <class NumberT>
void Put(std::string &out, NumberT number)
{
  const auto raw = std::bit_cast<std::array<char, sizeof number>>(number);
  out.append(raw.begin(), raw.end());
}

void Put(std::string &out, std::string_view text)
{
  out.append(text);
  out.push_back('\0');
}

void Code(std::string &out, bool wide, int group)
{
  if (wide)
  {
    Put(out, static_cast<std::uint16_t>(group));
    return;
  }
  if (group < 255)
  {
    Put(out, static_cast<std::uint8_t>(group));
    return;
  }
  Put(out, std::uint8_t{255});
  Put(out, static_cast<std::uint16_t>(group));
}

auto Sample(bool wide) -> std::string
{
  std::string out;
  Code(out, wide, 0);
  Put(out, std::string_view{"SECTION"});
  Code(out, wide, 70);
  Put(out, std::int16_t{-7});
  Code(out, wide, 90);
  Put(out, std::int32_t{70000});
  Code(out, wide, 160);
  Put(out, std::int64_t{1} << 40);
  Code(out, wide, 40);
  Put(out, 2.5);
  Code(out, wide, 290);
  Put(out, std::uint8_t{1});
  Code(out, wide, 5);
  Put(out, std::string_view{"1F"});
  Code(out, wide, 310);
  Put(out, std::uint8_t{3});
  out.append("\xde\xad\x01");
  Code(out, wide, 1071);
  Put(out, std::int32_t{-1});
  Code(out, wide, 0);
  Put(out, std::string_view{"EOF"});
  return out;
}

auto Read(std::string_view bytes, std::vector<Issue> *issues = nullptr) -> std::vector<Tag>
{
  Reader in{bytes, issues};
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

auto Refused(std::string_view bytes) -> Failure
{
  Reader in{bytes, nullptr};
  while (!in.Done())
  {
    const std::expected<std::optional<Tag>, Failure> one = in.Next();
    if (!one)
    {
      return one.error();
    }
  }
  FAIL("the bytes were read whole");
  return {};
}
} // namespace

TEST_CASE("binary: codes of two bytes and of one read the same tags")
{
  for (const bool wide : {true, false})
  {
    const std::string bytes = Sample(wide);
    const std::vector<Tag> tags = Read(bytes);
    REQUIRE(tags.size() == 10);
    CHECK(tags.at(0) == Tag{.Code = 0, .Value = std::string_view{"SECTION"}});
    CHECK(tags.at(1) == Tag{.Code = 70, .Value = std::int64_t{-7}});
    CHECK(tags.at(2) == Tag{.Code = 90, .Value = std::int64_t{70000}});
    CHECK(tags.at(3) == Tag{.Code = 160, .Value = std::int64_t{1} << 40});
    CHECK(tags.at(4) == Tag{.Code = 40, .Value = 2.5});
    CHECK(tags.at(5) == Tag{.Code = 290, .Value = true});
    CHECK(tags.at(6) == Tag{.Code = 5, .Value = std::string_view{"1F"}});
    CHECK(tags.at(7) == Tag{.Code = 310, .Value = Chunk{std::string_view{"\xde\xad\x01"}}});
    CHECK(tags.at(8) == Tag{.Code = 1071, .Value = std::int64_t{-1}});
    CHECK(tags.at(9) == Tag{.Code = 0, .Value = std::string_view{"EOF"}});
    CHECK(tags.at(9).Line == 0);
  }
}

TEST_CASE("binary: bytes cut short end the file there, told")
{
  std::string bytes = Sample(true);
  bytes.resize(bytes.size() - 2);
  std::vector<Issue> issues;
  CHECK(Read(bytes, &issues).size() == 9);
  CHECK(issues == std::vector{Issue{.Line = 0, .What = "group 0 cut short, the file ends there"}});
  std::vector<Issue> code;
  CHECK(Read(std::string_view{"\xff", 1}, &code).empty());
  CHECK(code == std::vector{Issue{.Line = 0, .What = "group code cut short, the file ends there"}});
}

TEST_CASE("binary: a real that is not finite is left out and told, the rest read on")
{
  for (const double odd : {std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity()})
  {
    std::string bytes;
    Code(bytes, true, 0);
    Put(bytes, std::string_view{"SECTION"});
    Code(bytes, true, 40);
    Put(bytes, odd);
    Code(bytes, true, 70);
    Put(bytes, std::int16_t{3});
    std::vector<Issue> issues;
    const std::vector<Tag> tags = Read(bytes, &issues);
    REQUIRE(tags.size() == 2);
    CHECK(tags.at(1) == Tag{.Code = 70, .Value = std::int64_t{3}});
    CHECK(issues == std::vector{Issue{.Line = 0, .What = "group 40 is no finite number, left out"}});
    CHECK(Read(bytes).size() == 2);
  }
}

TEST_CASE("binary: a group of a code DXF does not define is kept and told")
{
  std::string bytes;
  Code(bytes, true, 0);
  Put(bytes, std::string_view{"LINE"});
  Code(bytes, true, 250);
  Put(bytes, std::string_view{"2"});
  std::vector<Issue> issues;
  CHECK(Read(bytes, &issues).size() == 2);
  CHECK(issues == std::vector{Issue{.Line = 0, .What = "group 250 is no DXF group, kept"}});
}

TEST_CASE("binary: a code past the last there is is a failure")
{
  std::string bytes;
  Code(bytes, true, 0);
  Put(bytes, std::string_view{"SECTION"});
  Code(bytes, true, 4000);
  CHECK(Refused(bytes) == Failure{0, "group code expected, found 4000"});
}

TEST_CASE("binary: a value cut short ends the file there, whatever its type")
{
  for (const int group : {70, 90, 160, 40, 290, 310})
  {
    CAPTURE(group);
    std::string bytes;
    Code(bytes, true, 0);
    Put(bytes, std::string_view{"LINE"});
    Code(bytes, true, group);
    std::vector<Issue> issues;
    CHECK(Read(bytes, &issues).size() == 1);
    REQUIRE(issues.size() == 1);
    CHECK(issues.front().What == std::format("group {} cut short, the file ends there", group));
  }
  std::string chunk;
  Code(chunk, true, 0);
  Put(chunk, std::string_view{"LINE"});
  Code(chunk, true, 310);
  Put(chunk, std::uint8_t{4});
  chunk.append("\xff\xff");
  std::vector<Issue> issues;
  CHECK(Read(chunk, &issues).size() == 1);
  CHECK(issues == std::vector{Issue{.Line = 0, .What = "group 310 cut short, the file ends there"}});
}
} // namespace DxfIO::Tags::Binary
