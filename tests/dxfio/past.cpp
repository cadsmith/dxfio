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
      Entities::Seqend end{};
      end.Record.Base.Self = last;
      Entities::Insert out{};
      out.Attributes.Closing = Boxed<Entities::Seqend>{end};
      return out;
    }

    auto Order(Objects::Sortents::Entry one) -> Objects::SortEntsTable
    {
      Objects::SortEntsTable out{};
      out.Entries = {Objects::Sortents::Entry{}, one};
      return out;
    }

    template <class KindT>
    void Pinned(Tables::Table<KindT> Document::*member)
    {
      Document head{};
      (head.*member).Head.Base.Self = High;
      CHECK(Past(head) == Above);
      Document record{};
      (record.*member).Records.resize(2);
      Based((record.*member).Records.back()).Self = High;
      CHECK(Past(record) == Above);
    }

    auto Seeded(Group value) -> Header::Variable
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
      const Document made = Create(release);
      const auto declared = std::ranges::find(made.Header, "$HANDSEED", &Header::Variable::Name);
      REQUIRE(declared != made.Header.end());
      REQUIRE(declared->Values.size() == 1);
      CHECK(Past(made) == std::get<Handle>(declared->Values.front().Value));
    }
  }

  TEST_CASE("past: the seed's first handle for a document with none")
  {
    CHECK(Past(Document{}) == Seed{}.Next());
  }

  TEST_CASE("past: above the head and the records of every table")
  {
    Pinned(&Document::Vport);
    Pinned(&Document::Ltype);
    Pinned(&Document::Layer);
    Pinned(&Document::Style);
    Pinned(&Document::View);
    Pinned(&Document::Ucs);
    Pinned(&Document::Appid);
    Pinned(&Document::Dimstyle);
    Pinned(&Document::BlockRecord);
  }

  TEST_CASE("past: above a loose table's head and its records")
  {
    Document head{};
    head.Loose.resize(2);
    head.Loose.back().Head.Base.Self = High;
    CHECK(Past(head) == Above);
    Document within{};
    within.Loose.resize(1);
    within.Loose.back().Records.resize(1);
    within.Loose.back().Records.back().Base.Self = High;
    CHECK(Past(within) == Above);
  }

  TEST_CASE("past: above a block's two ends and what it holds")
  {
    Blocks::Block begin{};
    begin.Begin.Record.Base.Self = High;
    Blocks::Block end{};
    end.End.Record.Base.Self = High;
    Blocks::Block held{};
    held.Entities.emplace_back(Chain(High));
    for (const Blocks::Block &one : {begin, end, held})
    {
      Document drawing{};
      drawing.Blocks = {Blocks::Block{}, one};
      CHECK(Past(drawing) == Above);
    }
  }

  TEST_CASE("past: above an entity, its children and its SEQEND")
  {
    Document own{};
    own.Entities.emplace_back(Lone(High));
    CHECK(Past(own) == Above);
    Document child{};
    child.Entities.emplace_back(Chain(High));
    CHECK(Past(child) == Above);
    Document last{};
    last.Entities.emplace_back(Closed(High));
    CHECK(Past(last) == Above);
    Entities::Polyline closed = Chain(Handle{.Value = 2});
    closed.Vertices.Closing   = Boxed{Entities::Seqend{.Record = {.Base = {.Self = High}}}};
    Document both{};
    both.Entities.emplace_back(closed);
    CHECK(Past(both) == Above);
  }

  TEST_CASE("past: above an object and the handles of a draw order")
  {
    Objects::Dictionary held{};
    held.Base.Self = High;
    Document own{};
    own.Objects.emplace_back(held);
    CHECK(Past(own) == Above);
    Document key{};
    key.Objects.emplace_back(Order(Objects::Sortents::Entry{.Entity = {}, .Key = High}));
    CHECK(Past(key) == Above);
    Document named{};
    named.Objects.emplace_back(Order(Objects::Sortents::Entry{.Entity = High, .Key = {}}));
    CHECK(Past(named) == Above);
  }

  TEST_CASE("past: a $HANDSEED kept when higher, passed when stale")
  {
    Document drawing{};
    drawing.Header = {Seeded(Group{.Code = 5, .Value = High})};
    CHECK(Past(drawing) == High);
    drawing.Entities.emplace_back(Lone(High));
    CHECK(Past(drawing) == Above);
    drawing.Entities.emplace_back(Lone(Above));
    CHECK(Past(drawing) == Handle{.Value = 0x81});
  }

  TEST_CASE("past: a $HANDSEED that holds no handle, and others' handles")
  {
    Document drawing{};
    drawing.Header = {Seeded(Group{.Code = 5, .Value = std::string{"ZZ"}})};
    CHECK(Past(drawing) == Seed{}.Next());
    Header::Variable other = Seeded(Group{.Code = 5, .Value = High});
    other.Name             = "$CLAYER";
    drawing.Header         = {other};
    CHECK(Past(drawing) == Seed{}.Next());
  }

  TEST_CASE("past: none left once only the last handle is")
  {
    Document taken{};
    taken.Entities.emplace_back(Lone(Ceiling));
    CHECK_FALSE(Past(taken));
    taken.Header = {Seeded(Group{.Code = 5, .Value = High})};
    CHECK_FALSE(Past(taken));
    Document spare{};
    spare.Entities.emplace_back(Lone(Handle{.Value = Ceiling.Value - 1}));
    CHECK_FALSE(Past(spare));
    Document declared{};
    declared.Header = {Seeded(Group{.Code = 5, .Value = Ceiling})};
    CHECK_FALSE(Past(declared));
    Document room{};
    room.Entities.emplace_back(Lone(Handle{.Value = Ceiling.Value - 2}));
    CHECK(Past(room) == Handle{.Value = Ceiling.Value - 1});
  }
} // namespace DxfIO
