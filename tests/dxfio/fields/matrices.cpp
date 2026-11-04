#include "fields/matrices.hpp"

#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string>

namespace DxfIO::Fields
{
namespace
{
auto Run(double scale) -> std::string
{
  std::string out = " 43\n" + std::to_string(scale) + "\n";
  for (int at = 1; at < 16; ++at)
  {
    out += at % 5 == 0 ? " 43\n1.0\n" : " 43\n0.0\n";
  }
  return out;
}
} // namespace

TEST_CASE("matrices: sixteen reals on one code, in their order")
{
  const std::string text = Run(0.5) + " 44\n0.5\n";
  const auto all = Tags::All(text);
  REQUIRE(all);
  Walk in{*all, Reading{}};
  const auto taken = Value<Matrix>::Read(in, 43);
  REQUIRE(taken);
  CHECK(taken->Cells.front() == 0.5);
  CHECK(taken->Cells.at(5) == 1);
  CHECK(taken->Cells.back() == 1);
  CHECK_FALSE(Value<Matrix>::Read(in, 47));
}

TEST_CASE("matrices: a transform cut short keeps the identity's cells")
{
  const auto all = Tags::All(" 43\n2.0\n 43\n0.0\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Matrix wanted;
  wanted.Cells.front() = 2;
  CHECK(Value<Matrix>::Read(in, 43) == wanted);
}

TEST_CASE("matrices: written as its sixteen groups")
{
  Sheet page;
  REQUIRE(Value<Matrix>::Write(page.To, 43, Identity));
  std::string wanted;
  for (int at = 0; at < 16; ++at)
  {
    wanted += at % 5 == 0 ? " 43\r\n1.0\r\n" : " 43\r\n0.0\r\n";
  }
  CHECK(page.Out == wanted);
}

TEST_CASE("matrices: found wherever the field before it stopped")
{
  const auto all = Tags::All("100\nAcDbMaterial\n 43\n0.5\n 72\n1\n");
  REQUIRE(all);
  Walk in{*all, Reading{}};
  REQUIRE(in.Subclass("AcDbMaterial"));
  REQUIRE(in.Take(72) != nullptr);
  const auto taken = Value<Matrix>::Read(in, 43);
  REQUIRE(taken);
  CHECK(taken->Cells.front() == 0.5);
}
} // namespace DxfIO::Fields
