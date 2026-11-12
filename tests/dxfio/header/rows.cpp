#include "header/rows.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <set>
#include <string_view>

namespace DxfIO::Header::Rows
{
  namespace
  {
    constexpr std::array Releases{
        Version::R12,   Version::R13,   Version::R14,   Version::R2000, Version::R2004,
        Version::R2007, Version::R2010, Version::R2013, Version::R2018,
    };
  } // namespace

  TEST_CASE("rows: no release has a variable twice")
  {
    for (const Version release : Releases)
    {
      std::set<std::string_view> seen;
      for (const Row &one : All())
      {
        if (one.Since <= release && release <= one.Until)
        {
          CHECK(seen.insert(one.Name).second);
        }
      }
    }
  }

  TEST_CASE("rows: every release declares itself in its $ACADVER")
  {
    for (const Version release : Releases)
    {
      const auto found = std::ranges::find_if(
          All(), [release](const Row &one) -> bool
          { return one.Name == "$ACADVER" && one.Since <= release && release <= one.Until; });
      REQUIRE(found != All().end());
      const auto *text = std::get_if<std::string_view>(&found->Value);
      REQUIRE(text != nullptr);
      CHECK(DxfIO::Release(*text) == release);
    }
  }
} // namespace DxfIO::Header::Rows
