#include "dxfio/version.hpp"

#include <doctest/doctest.h>

#include <array>

namespace DxfIO
{
  TEST_CASE("version: every release reads back from its own code")
  {
    constexpr std::array all{Version::R10,   Version::R12,   Version::R13,   Version::R14,
                             Version::R2000, Version::R2004, Version::R2007, Version::R2010,
                             Version::R2013, Version::R2018};
    for (const Version one : all)
    {
      CHECK(ReleaseOf(AcadverOf(one)) == one);
    }
    CHECK(AcadverOf(Version::R12)   == "AC1009");
    CHECK(AcadverOf(Version::R2018) == "AC1032");
    CHECK(NameOf(Version::R2013)    == "R2013");
  }

  TEST_CASE("version: a code between two is the release before it")
  {
    CHECK(ReleaseOf("AC1006") == Version::R10);
    CHECK(ReleaseOf("AC1008") == Version::R10);
    CHECK(ReleaseOf("AC1009") == Version::R12);
    CHECK(ReleaseOf("AC1010") == Version::R12);
    CHECK(ReleaseOf("AC1016") == Version::R2000);
  }

  TEST_CASE("version: a code older than the oldest is the oldest")
  {
    CHECK(ReleaseOf("AC1002") == Version::R10);
  }

  TEST_CASE("version: a code past the newest is the newest")
  {
    CHECK(ReleaseOf("AC1033") == Version::R2018);
  }

  TEST_CASE("version: blanks around a code and its case do not matter")
  {
    CHECK(ReleaseOf("AC1027 ")  == Version::R2013);
    CHECK(ReleaseOf("\tAC1027") == Version::R2013);
    CHECK(ReleaseOf("ac1015")   == Version::R2000);
  }

  TEST_CASE("version: what is no AC code names no release")
  {
    CHECK_FALSE(ReleaseOf("R2000"));
    CHECK_FALSE(ReleaseOf("AC10X5"));
    CHECK_FALSE(ReleaseOf("AC101"));
    CHECK_FALSE(ReleaseOf("XX1015"));
    CHECK_FALSE(ReleaseOf(""));
  }

  TEST_CASE("version: releases compare oldest first")
  {
    CHECK(Version::R2004 < Version::R2007);
  }
} // namespace DxfIO
