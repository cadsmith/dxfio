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

    auto Seeding(Document &drawing) -> Header::Variable &
    {
      return *std::ranges::find(drawing.Header, "$HANDSEED", &Header::Variable::Name);
    }

    auto Seed(Document &drawing) -> Handle
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
    Document     drawing = Create();
    const Handle seed    = Seed(drawing);
    const auto   first   = NewHandle(drawing);
    const auto   second  = NewHandle(drawing);
    REQUIRE(first);
    REQUIRE(second);
    CHECK(*first        == seed);
    CHECK(second->Value == seed.Value + 1);
    CHECK(Seed(drawing).Value == seed.Value + 2);
  }

  TEST_CASE("edit: with no seed, a new handle comes past every handle held, and the seed is set")
  {
    Document drawing{};
    drawing.Entities.emplace_back(Lined(Handle{.Value = 0x40}));
    CHECK(NewHandle(drawing)      == Handle{.Value = 0x41});
    REQUIRE(drawing.Header.size() == 1);
    CHECK(Seeding(drawing).Values == std::vector{Group{.Code = 5, .Value = Handle{.Value = 0x42}}});
    CHECK(NewHandle(drawing)      == Handle{.Value = 0x42});
  }

  TEST_CASE("edit: a drawing whose $HANDSEED is gone gives a new handle each time")
  {
    Document drawing = Create(Version::R2018);
    std::erase_if(drawing.Header,
                  [](const Header::Variable &one) -> bool { return one.Name == "$HANDSEED"; });
    const auto first  = NewHandle(drawing);
    const auto second = NewHandle(drawing);
    REQUIRE(first);
    REQUIRE(second);
    CHECK(second->Value == first->Value + 1);
    REQUIRE(std::ranges::contains(drawing.Header, "$HANDSEED", &Header::Variable::Name));
    CHECK(Seed(drawing).Value == first->Value + 2);
  }

  TEST_CASE("edit: a seed that holds no handle counts as none, and takes the one after")
  {
    Document drawing{};
    drawing.Entities.emplace_back(Lined(Handle{.Value = 0x40}));
    drawing.Header.push_back(Header::Variable{.Name = "$HANDSEED", .Values = {}});
    CHECK(NewHandle(drawing) == Handle{.Value = 0x41});
    CHECK(Seed(drawing)      == Handle{.Value = 0x42});
    drawing.Header.front().Values.front() = Group{.Code = 5, .Value = std::string{"walls"}};
    CHECK(NewHandle(drawing) == Handle{.Value = 0x41});
    CHECK(std::get<Handle>(drawing.Header.front().Values.front().Value) == Handle{.Value = 0x42});
  }

  TEST_CASE("edit: past the last handle there is, no handle is given")
  {
    Document seeded                      = Create();
    Seeding(seeded).Values.front().Value = Handle{.Value = Last};
    const auto none                      = NewHandle(seeded);
    REQUIRE_FALSE(none);
    CHECK(none.error().What == "the drawing holds the last handle there is");

    Document full{};
    full.Entities.emplace_back(Lined(Handle{.Value = Last}));
    CHECK_FALSE(NewHandle(full));
  }
} // namespace DxfIO::Edit
