#include "dxfio/edit/find.hpp"

#include "ascii.hpp"

#include <algorithm>

namespace DxfIO::Edit
{
namespace
{
template <class DocumentT>
auto Entity(DocumentT &drawing, Handle self) -> auto *
{
  using Found = decltype(&drawing.Entities.front());
  if (!self)
  {
    return static_cast<Found>(nullptr);
  }
  const auto mine = [self](const DxfIO::Entity &one) -> bool
  {
    return BaseOf(one).Self == self;
  };
  const auto top = std::ranges::find_if(drawing.Entities, mine);
  if (top != drawing.Entities.end())
  {
    return &*top;
  }
  for (auto &block : drawing.Blocks)
  {
    const auto held = std::ranges::find_if(block.Entities, mine);
    if (held != block.Entities.end())
    {
      return &*held;
    }
  }
  return static_cast<Found>(nullptr);
}

template <class DocumentT>
auto Object(DocumentT &drawing, Handle self) -> auto *
{
  const auto found = !self ? drawing.Objects.end()
                           : std::ranges::find_if(drawing.Objects, [self](const DxfIO::Object &one) -> bool
                                                  { return BaseOf(one).Self == self; });
  return found == drawing.Objects.end() ? nullptr : &*found;
}

template <class DocumentT>
auto Block(DocumentT &drawing, std::string_view name) -> auto *
{
  const auto found = std::ranges::find_if(drawing.Blocks, [name](const Blocks::Block &one) -> bool
                                          { return Same(one.Begin.Name, name); });
  return found == drawing.Blocks.end() ? nullptr : &*found;
}
} // namespace

auto Same(std::string_view one, std::string_view other) -> bool
{
  return Ascii::Same(one, other);
}

auto FindEntity(Document &drawing, Handle self) -> DxfIO::Entity *
{
  return Entity(drawing, self);
}

auto FindEntity(const Document &drawing, Handle self) -> const DxfIO::Entity *
{
  return Entity(drawing, self);
}

auto FindObject(Document &drawing, Handle self) -> DxfIO::Object *
{
  return Object(drawing, self);
}

auto FindObject(const Document &drawing, Handle self) -> const DxfIO::Object *
{
  return Object(drawing, self);
}

auto FindBlock(Document &drawing, std::string_view name) -> Blocks::Block *
{
  return Block(drawing, name);
}

auto FindBlock(const Document &drawing, std::string_view name) -> const Blocks::Block *
{
  return Block(drawing, name);
}
} // namespace DxfIO::Edit
