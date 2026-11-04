#include "dxfio/version.hpp"

#include <doctest/doctest.h>

#include <array>

namespace DxfIO
{
TEST_CASE("version: every release reads back from its own code")
{
  constexpr std::array all{Version::R10,   Version::R12,   Version::R13,   Version::R14,   Version::R2000,
                           Version::R2004, Version::R2007, Version::R2010, Version::R2013, Version::R2018};
  for (const Version one : all)
  {
    CHECK(Release(Acadver(one)) == one);
  }
  CHECK(Acadver(Version::R12) == "AC1009");
  CHECK(Acadver(Version::R2018) == "AC1032");
  CHECK(Name(Version::R2013) == "R2013");
}

TEST_CASE("version: a code between two is the release before it")
{
  CHECK(Release("AC1006") == Version::R10);
  CHECK(Release("AC1008") == Version::R10);
  CHECK(Release("AC1009") == Version::R12);
  CHECK(Release("AC1010") == Version::R12);
  CHECK(Release("AC1016") == Version::R2000);
}

TEST_CASE("version: a code older than the oldest is the oldest")
{
  CHECK(Release("AC1002") == Version::R10);
}

TEST_CASE("version: a code past the newest is the newest")
{
  CHECK(Release("AC1033") == Version::R2018);
}

TEST_CASE("version: blanks around a code and its case do not matter")
{
  CHECK(Release("AC1027 ") == Version::R2013);
  CHECK(Release("\tAC1027") == Version::R2013);
  CHECK(Release("ac1015") == Version::R2000);
}

TEST_CASE("version: what is no AC code names no release")
{
  CHECK_FALSE(Release("R2000"));
  CHECK_FALSE(Release("AC10X5"));
  CHECK_FALSE(Release("AC101"));
  CHECK_FALSE(Release("XX1015"));
  CHECK_FALSE(Release(""));
}

TEST_CASE("version: releases compare oldest first")
{
  CHECK(Version::R2004 < Version::R2007);
}
} // namespace DxfIO
