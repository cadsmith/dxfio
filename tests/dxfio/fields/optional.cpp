#include "fields/optional.hpp"

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"
#include "tracked.hpp"

#include <doctest/doctest.h>

#include <optional>

namespace DxfIO::Fields
{
  namespace
  {
    struct Holder
    {
      std::optional<double> Width{1.5};
    };
  } // namespace

  TEST_CASE("optional: none when the record gives none, as every value")
  {
    const auto all = Tags::All("42\n1.5\n");
    REQUIRE(all);
    Walk in{*all, Reading{}};
    CHECK_FALSE(Value<std::optional<double>>::Read(in, 40));
  }

  TEST_CASE("optional: the value when given")
  {
    const auto all = Tags::All("40\n1.5\n");
    REQUIRE(all);
    Walk       in{*all, Reading{}};
    const auto taken = Value<std::optional<double>>::Read(in, 40);
    REQUIRE(taken);
    CHECK(taken.value_or(std::nullopt) == 1.5);
  }

  TEST_CASE("optional: a default kept unless a field falls back from it")
  {
    const auto all = Tags::All("42\n1.5\n");
    REQUIRE(all);
    Walk   kept{*all, Reading{}};
    Holder plain{};
    Take(Field{.Code = 40, .Member = &Holder::Width}, plain, kept);
    CHECK(plain.Width == 1.5);
    Walk   fallen{*all, Reading{}};
    Holder cleared{};
    Take(Field{.Code = 40, .Member = &Holder::Width, .Fallback = std::nullopt}, cleared, fallen);
    CHECK_FALSE(cleared.Width);
  }

  TEST_CASE("optional: written only when held")
  {
    Sheet page;
    REQUIRE(Value<std::optional<double>>::Write(page.To, 40, std::nullopt));
    CHECK(page.Out.empty());
    REQUIRE(Value<std::optional<double>>::Write(page.To, 40, 1.5));
    CHECK(page.Out == " 40\r\n1.5\r\n");
  }

  TEST_CASE("optional: the value read is moved in, not copied")
  {
    const auto all = Tags::All("  1\nx\n");
    REQUIRE(all);
    Walk                                        in{*all, Reading{}};
    const std::optional<std::optional<Tracked>> taken = Value<std::optional<Tracked>>::Read(in, 1);
    const Tracked                              *held  = taken && *taken ? &**taken : nullptr;
    REQUIRE(held != nullptr);
    CHECK_FALSE(held->Copied);
  }
} // namespace DxfIO::Fields
