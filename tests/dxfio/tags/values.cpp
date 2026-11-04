#include "tags/values.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <string_view>

namespace DxfIO::Tags
{
TEST_CASE("values: text, handles and chunks are kept as read")
{
  CHECK(Parse(Codes::Type::Text, "  a name ") == Content{std::string_view{"  a name "}});
  CHECK(Parse(Codes::Type::Text, "") == Content{std::string_view{}});
  CHECK(Parse(Codes::Type::Handle, "1F") == Content{std::string_view{"1F"}});
  CHECK(Parse(Codes::Type::Handle, "ARROW") == Content{std::string_view{"ARROW"}});
  CHECK(Parse(Codes::Type::Binary, "00FF10") == Content{std::string_view{"00FF10"}});
}

TEST_CASE("values: a chunk is hex digits in pairs, kept as read")
{
  CHECK(Parse(Codes::Type::Binary, "00ff10") == Content{std::string_view{"00ff10"}});
  CHECK(Parse(Codes::Type::Binary, "") == Content{std::string_view{}});
  CHECK(Parse(Codes::Type::Binary, "0FG0").error() == "hex");
  CHECK(Parse(Codes::Type::Binary, "ABC").error() == "hex");
  CHECK(Parse(Codes::Type::Binary, " 0F").error() == "hex");
}

TEST_CASE("values: a real is read whole, its blanks and one plus aside")
{
  CHECK(Parse(Codes::Type::Real, "1.5") == Content{1.5});
  CHECK(Parse(Codes::Type::Real, "  -0.25 ") == Content{-0.25});
  CHECK(Parse(Codes::Type::Real, "+2") == Content{2.0});
  CHECK(Parse(Codes::Type::Real, "1e3") == Content{1000.0});
  CHECK(Parse(Codes::Type::Real, "1.5x").error() == "number");
  CHECK_FALSE(Parse(Codes::Type::Real, "++1"));
  CHECK_FALSE(Parse(Codes::Type::Real, "+-1"));
  CHECK_FALSE(Parse(Codes::Type::Real, ""));
  CHECK_FALSE(Parse(Codes::Type::Real, "   "));
  CHECK_FALSE(Parse(Codes::Type::Real, "nan"));
  CHECK_FALSE(Parse(Codes::Type::Real, "inf"));
}

TEST_CASE("values: an integer is read whole, as AutoCAD pads it")
{
  CHECK(Parse(Codes::Type::Int16, "     1") == Content{std::int64_t{1}});
  CHECK(Parse(Codes::Type::Int32, "-42") == Content{std::int64_t{-42}});
  CHECK(Parse(Codes::Type::Int64, "9223372036854775807") == Content{std::int64_t{9223372036854775807}});
  CHECK(Parse(Codes::Type::Int16, "1x").error() == "integer");
  CHECK_FALSE(Parse(Codes::Type::Int16, ""));
}

TEST_CASE("values: an integer past the range of its group is what its type expected, not a value")
{
  CHECK(Parse(Codes::Type::Int16, "65535") == Content{std::int64_t{65535}});
  CHECK(Parse(Codes::Type::Int16, "-32768") == Content{std::int64_t{-32768}});
  CHECK(Parse(Codes::Type::Int16, "65536").error() == "integer of 16 bits");
  CHECK(Parse(Codes::Type::Int16, "-32769").error() == "integer of 16 bits");
  CHECK(Parse(Codes::Type::Int32, "2147483647") == Content{std::int64_t{2147483647}});
  CHECK(Parse(Codes::Type::Int32, "-2147483648") == Content{std::int64_t{-2147483648}});
  CHECK(Parse(Codes::Type::Int32, "2147483648").error() == "integer of 32 bits");
  CHECK(Parse(Codes::Type::Int32, "-2147483649").error() == "integer of 32 bits");
  CHECK(Parse(Codes::Type::Int64, "2147483648") == Content{std::int64_t{2147483648}});
  CHECK(Parse(Codes::Type::Boolean, "65536") == Content{true});
}

TEST_CASE("values: an integer given as a real is truncated")
{
  CHECK(Parse(Codes::Type::Int16, "1.0") == Content{std::int64_t{1}});
  CHECK(Parse(Codes::Type::Int16, "-2.7") == Content{std::int64_t{-2}});
  CHECK_FALSE(Parse(Codes::Type::Int64, "1e300"));
  CHECK_FALSE(Parse(Codes::Type::Int64, "-1e19"));
}

TEST_CASE("values: a boolean is an integer that is not 0")
{
  CHECK(Parse(Codes::Type::Boolean, "     1") == Content{true});
  CHECK(Parse(Codes::Type::Boolean, "0") == Content{false});
  CHECK(Parse(Codes::Type::Boolean, "2") == Content{true});
  CHECK_FALSE(Parse(Codes::Type::Boolean, "yes"));
}
} // namespace DxfIO::Tags
