#include "../family.hpp"
#include "entities/io.hpp"
#include "masked.hpp"
#include "match.hpp"
#include "shapes/arc.hpp"
#include "shapes/attdef.hpp"
#include "shapes/circle.hpp"
#include "shapes/dimension/dimension.hpp"
#include "shapes/ellipse.hpp"
#include "shapes/face.hpp"
#include "shapes/hatch/hatch.hpp"
#include "shapes/helix.hpp"
#include "shapes/insert.hpp"
#include "shapes/leader.hpp"
#include "shapes/light.hpp"
#include "shapes/line.hpp"
#include "shapes/lwpolyline.hpp"
#include "shapes/mesh.hpp"
#include "shapes/mleader.hpp"
#include "shapes/mline.hpp"
#include "shapes/modeler.hpp"
#include "shapes/mtext.hpp"
#include "shapes/ole.hpp"
#include "shapes/point.hpp"
#include "shapes/polyline.hpp"
#include "shapes/raster.hpp"
#include "shapes/ray.hpp"
#include "shapes/shape.hpp"
#include "shapes/slice.hpp"
#include "shapes/solid.hpp"
#include "shapes/spline.hpp"
#include "shapes/table.hpp"
#include "shapes/text.hpp"
#include "shapes/tolerance.hpp"
#include "shapes/trace.hpp"
#include "shapes/underlay.hpp"
#include "shapes/viewport.hpp"
#include "shapes/xline.hpp"
#include "tags/codes.hpp"

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace DxfIO::Family
{
template <>
inline constexpr bool Ancient<Entities::Point> = true;

template <>
inline constexpr bool Ancient<Entities::Line> = true;

template <>
inline constexpr bool Ancient<Entities::Circle> = true;

template <>
inline constexpr bool Ancient<Entities::Arc> = true;

template <>
inline constexpr bool Ancient<Entities::Text> = true;

template <>
inline constexpr bool Ancient<Entities::Shape> = true;

template <>
inline constexpr bool Ancient<Entities::Solid> = true;

template <>
inline constexpr bool Ancient<Entities::Trace> = true;

template <>
inline constexpr bool Ancient<Entities::Face3d> = true;

template <>
inline constexpr bool Ancient<Entities::Attdef> = true;

template <>
inline constexpr bool Ancient<Entities::Insert> = true;

template <>
inline constexpr bool Ancient<Entities::Polyline> = true;

template <>
inline constexpr bool Ancient<Entities::Dimension> = true;

template <>
inline constexpr bool Ancient<Entities::Viewport> = true;
} // namespace DxfIO::Family

namespace DxfIO::Entities
{
namespace
{
constexpr int Sheet = 67;
constexpr std::string_view Common = "AcDbEntity";

auto Opens(const Group &one) -> bool
{
  return one.Code == Tags::Codes::Marker && one.Value == Content{std::string{Common}};
}

template <class GroupsT>
auto Fence(GroupsT &all)
{
  const auto marker = std::ranges::find_if(all, Opens);
  const auto first = marker == all.end() ? all.begin() : std::next(marker);
  const auto last = std::ranges::find(first, all.end(), Tags::Codes::Marker, &Group::Code);
  return std::ranges::subrange(first, last);
}

auto Flagged(const Group &one) -> bool
{
  return one.Code == Sheet && one.Value != Content{std::int64_t{0}};
}

void Flag(std::vector<Group> &all)
{
  const Group raised{.Code = Sheet, .Value = std::int64_t{1}};
  auto held = Fence(all) | std::views::filter([](const Group &one) -> bool { return one.Code == Sheet; });
  if (std::ranges::empty(held))
  {
    all.insert(Fence(all).begin(), raised);
    return;
  }
  std::ranges::fill(held, raised);
}

template <class ChainT, class VisitT>
void Walked(ChainT &chain, const VisitT &visit)
{
  for (auto &child : chain.Children)
  {
    visit(child.Record);
  }
  if (chain.Closing)
  {
    visit((**chain.Closing).Record);
  }
}

template <class KindT, class VisitT>
void Children(KindT &kind, const VisitT &visit)
{
  if constexpr (std::same_as<std::remove_const_t<KindT>, Polyline>)
  {
    Walked(kind.Vertices, visit);
  }
  if constexpr (std::same_as<std::remove_const_t<KindT>, Insert>)
  {
    Walked(kind.Attributes, visit);
  }
}
} // namespace

auto Read(std::span<const File::Record> all, const Fields::Reading &with) -> List
{
  List out = Family::Read<Entity>(all, with);
  out.shrink_to_fit();
  return out;
}

auto Named(const Entity &one) -> std::string_view
{
  return std::visit(
      DxfIO::Match{
          [](const Unknown &kept) -> std::string_view { return kept.Type.View(); },
          []<class KindT>(const KindT &) -> std::string_view { return Type<KindT>; },
      },
      one);
}

auto Based(const Entity &one) -> const DxfIO::Base &
{
  return std::visit([](const auto &kind) -> const DxfIO::Base & { return DxfIO::Based(kind); }, one);
}

auto Paper(const Entity &one) -> bool
{
  return std::visit(
      DxfIO::Match{
          [](const Unknown &kept) -> bool { return std::ranges::any_of(Fence(kept.Groups), Flagged); },
          [](const auto &kind) -> auto { return kind.Record.Paper; },
      },
      one);
}

auto Top(const Entity &one) -> Handle
{
  Handle out = Based(one).Self;
  const auto higher = [&out](const Record &child) -> void
  {
    out = std::max(out, child.Base.Self);
  };
  std::visit([&higher](const auto &kind) -> auto { Children(kind, higher); }, one);
  return out;
}

auto Asked(const Polyline &parent, const Vertex &one) -> int
{
  constexpr int Spatial = 8;
  constexpr int Meshed = 16;
  constexpr int Faced = 64;
  constexpr int Space = 32;
  constexpr int Mesh = 64;
  constexpr int Face = 128;
  const int kept = one.Flags - Masked(one.Flags, Space + Mesh + Face);
  if (Masked(parent.Flags, Faced) != 0)
  {
    const bool face = one.First != 0 || one.Second != 0 || one.Third != 0 || one.Fourth != 0;
    return kept + (face ? Face : Mesh + Face);
  }
  if (Masked(parent.Flags, Meshed) != 0)
  {
    return kept + Mesh;
  }
  return Masked(parent.Flags, Spatial) != 0 ? kept + Space : kept;
}

auto Attributing(const Entity &one) -> bool
{
  constexpr int Constant = 2;
  const auto *definition = std::get_if<Attdef>(&one);
  return definition != nullptr && Masked(definition->Flags, Constant) == 0;
}

void Claim(Entity &one, Handle owner)
{
  std::visit(
      DxfIO::Match{
          [owner](Unknown &kept) -> void
          {
            kept.Base.Owner = owner;
            Flag(kept.Groups);
          },
          [owner](auto &kind) -> auto
          {
            kind.Record.Base.Owner = owner;
            kind.Record.Paper = true;
            Children(kind, [](Record &child) -> void { child.Paper = true; });
          },
      },
      one);
}

void Own(Entity &one, Seed &handles, Handle owner)
{
  const auto held = [&handles](DxfIO::Base &base, Handle parent) -> void
  {
    if (!base.Self)
    {
      base.Self = handles.Take();
    }
    base.Owner = parent;
  };
  std::visit(
      DxfIO::Match{
          [&held, owner](Unknown &kept) -> void { held(kept.Base, owner); },
          [&held, owner](auto &kind) -> void
          {
            held(kind.Record.Base, owner);
            const Handle self = kind.Record.Base.Self;
            Children(kind, [&held, self](Record &child) -> void { held(child.Base, self); });
          },
      },
      one);
}

void Bare(Entity &one, bool extended)
{
  const auto bared = [extended](DxfIO::Base &base) -> void
  {
    DxfIO::Base kept{};
    kept.Self = base.Self;
    if (extended)
    {
      kept.Extended = std::move(base.Extended);
    }
    base = std::move(kept);
  };
  std::visit(
      DxfIO::Match{
          [&bared](Unknown &kept) -> void { bared(kept.Base); },
          [&bared](auto &kind) -> void
          {
            bared(kind.Record.Base);
            Children(kind, [&bared](Record &child) -> void { bared(child.Base); });
          },
      },
      one);
}

void Loose(Entity &one, const std::set<Handle> &gone)
{
  const auto loosened = [&gone](DxfIO::Base &base) -> void
  {
    base.Dictionary = {};
    std::erase_if(base.Reactors, [&gone](Handle held) -> bool { return gone.contains(held); });
  };
  std::visit(
      DxfIO::Match{
          [&loosened](Unknown &kept) -> void { loosened(kept.Base); },
          [&loosened](auto &kind) -> void
          {
            loosened(kind.Record.Base);
            Children(kind, [&loosened](Record &child) -> void { loosened(child.Base); });
          },
      },
      one);
}

auto Write(Fields::Writing &to, std::span<const Entity> all) -> std::expected<void, Failure>
{
  return Family::Write<Entity>(to, all);
}
} // namespace DxfIO::Entities
