#include "family.hpp"

#include "fields/fixture.hpp"
#include "file/sections/records.hpp"
#include "objects/shapes/placeholder.hpp"
#include "objects/shapes/scale.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <array>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO::Family
{
  namespace
  {
    using Small = std::variant<Objects::Scale, Objects::Placeholder, Unknown>;

    constexpr std::string_view Text = "  0\r\nSCALE\r\n  5\r\n10D\r\n330\r\n10C\r\n"
                                      "100\r\nAcDbScale\r\n 70\r\n     0\r\n"
                                      "300\r\n1:1\r\n140\r\n1.0\r\n"
                                      "141\r\n1.0\r\n290\r\n     1\r\n"
                                      "  0\r\nACDBPLACEHOLDER\r\n  5\r\nF\r\n330\r\nE\r\n"
                                      "  0\r\nACDBNOTHING\r\n  5\r\n9\r\n330\r\nC\r\n"
                                      " 90\r\n        7\r\n";

    auto Parsed() -> std::vector<Small>
    {
      const auto all = Tags::All(Text);
      REQUIRE(all);
      const auto records = File::Sections::Records(*all);
      REQUIRE(records);
      std::vector<Issue> issues;
      std::vector<Small> out = Read<Small>(*records, Fields::Reading{.Issues = &issues});
      CHECK(issues.empty());
      return out;
    }
  } // namespace

  TEST_CASE("family: each record read by the kind its type names")
  {
    const std::vector<Small> all = Parsed();
    REQUIRE(all.size() == 3);
    CHECK(std::get<Objects::Scale>(all.at(0)).Name == "1:1");
    CHECK(std::holds_alternative<Objects::Placeholder>(all.at(1)));
    CHECK(std::get<Unknown>(all.at(2)).Type == "ACDBNOTHING");
  }

  TEST_CASE("family: each value written under its type, as it was read")
  {
    const std::vector<Small> all = Parsed();
    CHECK(Type(all.at(0)) == "SCALE");
    CHECK(Type(all.at(1)) == "ACDBPLACEHOLDER");
    CHECK(Type(all.at(2)) == "ACDBNOTHING");
    Fields::Sheet page;
    REQUIRE(Write<Small>(page.To, all));
    CHECK(page.Out == Text);
  }

  TEST_CASE("family: its types named but the last, none twice")
  {
    static_assert(Distinct(std::array<std::string_view, 3>{"SCALE", "LAYOUT", ""}));
    static_assert(!Distinct(std::array<std::string_view, 3>{"SCALE", "SCALE", ""}));
    static_assert(!Distinct(std::array<std::string_view, 3>{"SCALE", "", ""}));
    static_assert(!Distinct(std::array<std::string_view, 2>{"SCALE", "LAYOUT"}));
    CHECK(Kinds<Small>.size() == 3);
  }
} // namespace DxfIO::Family
