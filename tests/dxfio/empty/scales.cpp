#include "empty/scales.hpp"

#include <doctest/doctest.h>

#include <set>

namespace DxfIO::Empty
{
  TEST_CASE("scales: AutoCAD's seventeen, 1:1 the one to one alone")
  {
    Seed                              handles;
    const std::vector<Objects::Scale> all = Scales(handles);
    REQUIRE(all.size()     == 17);
    CHECK(all.front().Name == "1:1");
    CHECK(all.front().Unit);
    CHECK(all.at(1).Name    == "1:2");
    CHECK(all.at(1).Drawing == 2);
    CHECK_FALSE(all.at(1).Unit);
    CHECK(all.back().Name  == "100:1");
    CHECK(all.back().Paper == 100);
    std::set<Handle> seen;
    for (const Objects::Scale &one : all)
    {
      CHECK(seen.insert(one.Base.Self).second);
    }
  }
} // namespace DxfIO::Empty
