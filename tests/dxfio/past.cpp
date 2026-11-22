#include "past.hpp"

#include "based.hpp"
#include "seed.hpp"

#include "dxfio/create.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <variant>

namespace DxfIO
{
  namespace
  {
    constexpr Handle High{.Value = 0x7f};
    constexpr Handle Above{.Value = 0x80};
    constexpr Handle Ceiling{.Value = std::numeric_limits<std::uint64_t>::max()};

    auto Lone(Handle self) -> Entities::Line
    {
      Entities::Line out{};
      out.Record.Base.Self = self;
      return out;
    }

    auto Chain(Handle child) -> Entities::Polyline
    {
      Entities::Polyline out{};
      out.Vertices.Children.resize(2);
      out.Vertices.Children.back().Record.Base.Self = child;
      return out;
    }

    auto Closed(Handle last) -> Entities::Insert
    {
      Entities::SeqEnd end{};
      end.Record.Base.Self = last;
      Entities::Insert out{};
      out.Attributes.Closing = Boxed<Entities::SeqEnd>{end};
      return out;
    }

    auto Order(Objects::SortEntsTables::Entry one) -> Objects::SortEntsTable
    {
      Objects::SortEntsTable out{};
      out.Entries = {Objects::SortEntsTables::Entry{}, one};
      return out;
    }

    template <class KindT>
    void Pinned(Tables::Table<KindT> Drawing::*member)
    {
      Drawing head{};
      (head.*member).Control.Base.Self = High;
      CHECK(Past(head) == Above);
      Drawing record{};
      (record.*member).Records.resize(2);
      Based((record.*member).Records.back()).Self = High;
      CHECK(Past(record) == Above);
    }

    auto Seeded(Tag value) -> Header::Variable
    {
      value.Code = 5;
      return {.Name = "$HANDSEED", .Values = {std::move(value)}};
    }
  } // namespace

  TEST_CASE("past: a new drawing's own seed, in every release")
  {
    const std::array some{Version::R12, Version::R2000, Version::R2018};
    for (const Version release : some)
    {
      CAPTURE(release);
      const Drawing made     = Create(release);
      const auto    declared = std::ranges::find(made.Header, "$HANDSEED", &Header::Variable::Name);
      REQUIRE(declared != made.Header.end());
      REQUIRE(declared->Values.size() == 1);
      CHECK(Past(made) == std::get<Handle>(declared->Values.front().Value));
    }
  }

  TEST_CASE("past: the seed's first handle for a drawing with none")
  {
    CHECK(Past(Drawing{}) == Seed{}.Next());
  }

  TEST_CASE("past: above the head and the records of every table")
  {
    Pinned(&Drawing::VPort);
    Pinned(&Drawing::LType);
    Pinned(&Drawing::Layer);
    Pinned(&Drawing::Style);
    Pinned(&Drawing::View);
    Pinned(&Drawing::Ucs);
    Pinned(&Drawing::AppId);
    Pinned(&Drawing::DimStyle);
    Pinned(&Drawing::BlockRecord);
  }

  TEST_CASE("past: above a loose table's head and its records")
  {
    Drawing head{};
    head.Loose.resize(2);
    head.Loose.back().Control.Base.Self = High;
    CHECK(Past(head) == Above);
    Drawing within{};
    within.Loose.resize(1);
    within.Loose.back().Records.resize(1);
    within.Loose.back().Records.back().Base.Self = High;
    CHECK(Past(within) == Above);
  }

  TEST_CASE("past: above a block's two ends and what it holds")
  {
    Blocks::Definition begin{};
    begin.Block.Record.Base.Self = High;
    Blocks::Definition end{};
    end.EndBlk.Record.Base.Self = High;
    Blocks::Definition held{};
    held.Entities.emplace_back(Chain(High));
    for (const Blocks::Definition &one : {begin, end, held})
    {
      Drawing drawing{};
      drawing.Blocks = {Blocks::Definition{}, one};
      CHECK(Past(drawing) == Above);
    }
  }

  TEST_CASE("past: above an entity, its children and its SEQEND")
  {
    Drawing own{};
    own.Entities.emplace_back(Lone(High));
    CHECK(Past(own) == Above);
    Drawing child{};
    child.Entities.emplace_back(Chain(High));
    CHECK(Past(child) == Above);
    Drawing last{};
    last.Entities.emplace_back(Closed(High));
    CHECK(Past(last) == Above);
    Entities::Polyline closed = Chain(Handle{.Value = 2});
    closed.Vertices.Closing   = Boxed{Entities::SeqEnd{.Record = {.Base = {.Self = High}}}};
    Drawing both{};
    both.Entities.emplace_back(closed);
    CHECK(Past(both) == Above);
  }

  TEST_CASE("past: above an object and the handles of a draw order")
  {
    Objects::Dictionary held{};
    held.Base.Self = High;
    Drawing own{};
    own.Objects.emplace_back(held);
    CHECK(Past(own) == Above);
    Drawing key{};
    key.Objects.emplace_back(Order(Objects::SortEntsTables::Entry{.Entity = {}, .Key = High}));
    CHECK(Past(key) == Above);
    Drawing named{};
    named.Objects.emplace_back(Order(Objects::SortEntsTables::Entry{.Entity = High, .Key = {}}));
    CHECK(Past(named) == Above);
  }

  TEST_CASE("past: a $HANDSEED kept when higher, passed when stale")
  {
    Drawing drawing{};
    drawing.Header = {Seeded(Tag{.Code = 5, .Value = High})};
    CHECK(Past(drawing) == High);
    drawing.Entities.emplace_back(Lone(High));
    CHECK(Past(drawing) == Above);
    drawing.Entities.emplace_back(Lone(Above));
    CHECK(Past(drawing) == Handle{.Value = 0x81});
  }

  TEST_CASE("past: a $HANDSEED that holds no handle, and others' handles")
  {
    Drawing drawing{};
    drawing.Header = {Seeded(Tag{.Code = 5, .Value = std::string{"ZZ"}})};
    CHECK(Past(drawing) == Seed{}.Next());
    Header::Variable other = Seeded(Tag{.Code = 5, .Value = High});
    other.Name             = "$CLAYER";
    drawing.Header         = {other};
    CHECK(Past(drawing) == Seed{}.Next());
  }

  TEST_CASE("past: none left once only the last handle is")
  {
    Drawing taken{};
    taken.Entities.emplace_back(Lone(Ceiling));
    CHECK_FALSE(Past(taken));
    taken.Header = {Seeded(Tag{.Code = 5, .Value = High})};
    CHECK_FALSE(Past(taken));
    Drawing spare{};
    spare.Entities.emplace_back(Lone(Handle{.Value = Ceiling.Value - 1}));
    CHECK_FALSE(Past(spare));
    Drawing declared{};
    declared.Header = {Seeded(Tag{.Code = 5, .Value = Ceiling})};
    CHECK_FALSE(Past(declared));
    Drawing room{};
    room.Entities.emplace_back(Lone(Handle{.Value = Ceiling.Value - 2}));
    CHECK(Past(room) == Handle{.Value = Ceiling.Value - 1});
  }
} // namespace DxfIO
