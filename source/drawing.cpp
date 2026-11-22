#include "dxfio/drawing.hpp"

#include <algorithm>

namespace DxfIO
{
  namespace
  {
    template <class DrawingT>
    auto EntityIn(DrawingT &drawing, Handle self) -> auto *
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

    template <class DrawingT>
    auto ObjectIn(DrawingT &drawing, Handle self) -> auto *
    {
      const auto found =
          !self ? drawing.Objects.end()
                : std::ranges::find_if(drawing.Objects, [self](const DxfIO::Object &one) -> bool
                                       { return BaseOf(one).Self == self; });
      return found == drawing.Objects.end() ? nullptr : &*found;
    }
  } // namespace

  auto FindEntity(Drawing &drawing, Handle self) -> DxfIO::Entity *
  {
    return EntityIn(drawing, self);
  }

  auto FindEntity(const Drawing &drawing, Handle self) -> const DxfIO::Entity *
  {
    return EntityIn(drawing, self);
  }

  auto FindObject(Drawing &drawing, Handle self) -> DxfIO::Object *
  {
    return ObjectIn(drawing, self);
  }

  auto FindObject(const Drawing &drawing, Handle self) -> const DxfIO::Object *
  {
    return ObjectIn(drawing, self);
  }
} // namespace DxfIO
