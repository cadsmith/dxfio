#include "fields/maybe.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <optional>
#include <string_view>
#include <tuple>

namespace DxfIO::Fields
{
namespace
{
struct Inner
{
  int First{};
  int Second{};
};

struct Outer
{
  std::optional<Inner> Held;
};

const std::tuple Parts{
    Field{.Code = 70, .Member = &Inner::First},
    Field{.Code = 71, .Member = &Inner::Second},
};

const std::tuple Shapes{
    Maybe{.Opener = 70, .Member = &Outer::Held, .Fields = Parts},
};

auto Read(std::string_view text) -> Outer
{
  const auto all = Tags::All(text);
  REQUIRE(all);
  Walk in{*all, Reading{}};
  Outer out{};
  Fields::Read(out, in, Shapes);
  return out;
}
} // namespace

TEST_CASE("maybe: read when its opener is there")
{
  const Outer one = Read(" 70\n1\n 71\n2\n");
  REQUIRE(one.Held);
  CHECK(one.Held->Second == 2);
}

TEST_CASE("maybe: none without its opener")
{
  CHECK_FALSE(Read(" 71\n2\n").Held);
}

TEST_CASE("maybe: written when held")
{
  Sheet some;
  REQUIRE(Write(Outer{.Held = Inner{.First = 1, .Second = 2}}, some.To, Shapes));
  CHECK(some.Out == " 70\r\n     1\r\n 71\r\n     2\r\n");
  Sheet none;
  REQUIRE(Write(Outer{}, none.To, Shapes));
  CHECK(none.Out.empty());
}
} // namespace DxfIO::Fields
