#include "fields/custom.hpp"

#include "fields/numbers.hpp"
#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <tuple>

namespace DxfIO::Fields
{
  namespace
  {
    struct Counted
    {
      int Doubled{};
    };

    void Halve(Counted &owner, Walk &in)
    {
      owner.Doubled = 2 * Value<int>::Read(in, 70).value_or(0);
    }

    auto Twice(const Counted &owner, Writing &to) -> std::expected<void, Failure>
    {
      return Value<int>::Write(to, 70, owner.Doubled / 2);
    }

    const std::tuple Shapes{Custom<Counted>{.Read = Halve, .Write = Twice}};
  } // namespace

  TEST_CASE("custom: a function each way for what no shape says")
  {
    const auto all = Tags::All(" 70\n3\n");
    REQUIRE(all);
    Walk    in{*all, Reading{}};
    Counted taken{};
    Read(taken, in, Shapes);
    CHECK(taken.Doubled == 6);
    Sheet page;
    REQUIRE(Write(taken, page.To, Shapes));
    CHECK(page.Out == " 70\r\n     3\r\n");
  }
} // namespace DxfIO::Fields
