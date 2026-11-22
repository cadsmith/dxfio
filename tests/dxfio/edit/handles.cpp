#include "dxfio/edit/handles.hpp"

#include "dxfio/create.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>
#include <variant>
#include <vector>

namespace DxfIO::Edit
{
  namespace
  {
    constexpr std::uint64_t Last = std::numeric_limits<std::uint64_t>::max();

    auto Seeding(Drawing &drawing) -> Header::Variable &
    {
      return *std::ranges::find(drawing.Header, "$HANDSEED", &Header::Variable::Name);
    }

    auto Seed(Drawing &drawing) -> Handle
    {
      return std::get<Handle>(Seeding(drawing).Values.front().Value);
    }

    auto Lined(Handle self) -> Entities::Line
    {
      Entities::Line out{};
      out.Record.Base.Self = self;
      return out;
    }
  } // namespace

  TEST_CASE("edit: a new handle is the seed, and the seed moves past it")
  {
    Drawing      drawing = Create();
    const Handle seed    = Seed(drawing);
    const auto   first   = MakeHandle(drawing);
    const auto   second  = MakeHandle(drawing);
    REQUIRE(first);
    REQUIRE(second);
    CHECK(*first        == seed);
    CHECK(second->Value == seed.Value + 1);
    CHECK(Seed(drawing).Value == seed.Value + 2);
  }

  TEST_CASE("edit: with no seed, a new handle comes past every handle held, and the seed is set")
  {
    Drawing drawing{};
    drawing.Entities.emplace_back(Lined(Handle{.Value = 0x40}));
    CHECK(MakeHandle(drawing)     == Handle{.Value = 0x41});
    REQUIRE(drawing.Header.size() == 1);
    CHECK(Seeding(drawing).Values == std::vector{Tag{.Code = 5, .Value = Handle{.Value = 0x42}}});
    CHECK(MakeHandle(drawing)     == Handle{.Value = 0x42});
  }

  TEST_CASE("edit: a drawing whose $HANDSEED is gone gives a new handle each time")
  {
    Drawing drawing = Create(Version::R2018);
    std::erase_if(drawing.Header,
                  [](const Header::Variable &one) -> bool { return one.Name == "$HANDSEED"; });
    const auto first  = MakeHandle(drawing);
    const auto second = MakeHandle(drawing);
    REQUIRE(first);
    REQUIRE(second);
    CHECK(second->Value == first->Value + 1);
    REQUIRE(std::ranges::contains(drawing.Header, "$HANDSEED", &Header::Variable::Name));
    CHECK(Seed(drawing).Value == first->Value + 2);
  }

  TEST_CASE("edit: a seed that holds no handle counts as none, and takes the one after")
  {
    Drawing drawing{};
    drawing.Entities.emplace_back(Lined(Handle{.Value = 0x40}));
    drawing.Header.push_back(Header::Variable{.Name = "$HANDSEED", .Values = {}});
    CHECK(MakeHandle(drawing) == Handle{.Value = 0x41});
    CHECK(Seed(drawing)       == Handle{.Value = 0x42});
    drawing.Header.front().Values.front() = Tag{.Code = 5, .Value = std::string{"walls"}};
    CHECK(MakeHandle(drawing) == Handle{.Value = 0x41});
    CHECK(std::get<Handle>(drawing.Header.front().Values.front().Value) == Handle{.Value = 0x42});
  }

  TEST_CASE("edit: past the last handle there is, no handle is given")
  {
    Drawing seeded                       = Create();
    Seeding(seeded).Values.front().Value = Handle{.Value = Last};
    const auto none                      = MakeHandle(seeded);
    REQUIRE_FALSE(none);
    CHECK(none.error().What == "no handle left past the last one");

    Drawing full{};
    full.Entities.emplace_back(Lined(Handle{.Value = Last}));
    CHECK_FALSE(MakeHandle(full));
  }
} // namespace DxfIO::Edit
