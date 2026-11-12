#include "fields/tally.hpp"

#include "fields/many.hpp"
#include "fields/read.hpp"
#include "fixture.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <tuple>
#include <vector>

namespace DxfIO::Fields
{
  namespace
  {
    struct List
    {
      std::vector<double> Items;
    };

    const std::tuple Shapes{
        Tally{.Code = 72, .Member = &List::Items},
        Many{.Code = 40, .Member = &List::Items},
    };

    auto Read(std::string_view text) -> List
    {
      const auto all = Tags::All(text);
      REQUIRE(all);
      Walk in{*all, Reading{}};
      List out{};
      Fields::Read(out, in, Shapes);
      CHECK(in.Rest().empty());
      return out;
    }
  } // namespace

  TEST_CASE("tally: the list as long as its own groups, whatever the count")
  {
    CHECK(Read(" 72\n1\n 40\n1.0\n 40\n2.0\n").Items.size() == 2);
    CHECK(Read(" 72\n9\n 40\n1.0\n").Items.size()           == 1);
    CHECK(Read(" 40\n1.0\n").Items.size() == 1);
  }

  TEST_CASE("tally: written as the list's size")
  {
    Sheet page;
    REQUIRE(Write(List{.Items = {1.0, 2.0}}, page.To, Shapes));
    CHECK(page.Out == " 72\r\n     2\r\n 40\r\n1.0\r\n 40\r\n2.0\r\n");
  }
} // namespace DxfIO::Fields
