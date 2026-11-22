#include "handseed.hpp"

#include <doctest/doctest.h>

#include <string>
#include <utility>
#include <vector>

namespace DxfIO
{
  namespace
  {
    constexpr Handle Seeded{.Value = 0x2a};

    auto Declared(std::vector<Tag> values) -> Drawing
    {
      Drawing out{};
      out.Header = {Header::Variable{.Name = "$ACADVER", .Values = {}},
                    Header::Variable{.Name = "$HANDSEED", .Values = std::move(values)}};
      return out;
    }
  } // namespace

  TEST_CASE("handseed: read from its first group, none when it holds no handle")
  {
    CHECK(ReadSeed(Declared({Tag{.Code = 5, .Value = Seeded}})) == Seeded);
    CHECK_FALSE(ReadSeed(Declared({})));
    CHECK_FALSE(ReadSeed(Declared({Tag{.Code = 5, .Value = std::string{"2A"}}})));
    CHECK_FALSE(ReadSeed(Drawing{}));
  }

  TEST_CASE("handseed: written as its one group 5, added at the end when missing")
  {
    Drawing held =
        Declared({Tag{.Code = 5, .Value = std::string{"2A"}}, Tag{.Code = 5, .Value = Handle{}}});
    WriteSeed(held, Seeded);
    REQUIRE(held.Header.size()      == 2);
    CHECK(held.Header.back().Values == std::vector{Tag{.Code = 5, .Value = Seeded}});

    Drawing missing{};
    missing.Header = {Header::Variable{.Name = "$ACADVER", .Values = {}}};
    WriteSeed(missing, Seeded);
    REQUIRE(missing.Header.size()    == 2);
    CHECK(missing.Header.back().Name == "$HANDSEED");
    CHECK(ReadSeed(missing)          == Seeded);
  }
} // namespace DxfIO
