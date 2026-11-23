#include "dxfio/view.hpp"

#include "ascii.hpp"

#include <algorithm>
#include <type_traits>
#include <variant>
#include <vector>

namespace DxfIO::View
{
  namespace
  {
    constexpr std::string_view Model = "*Model_Space";

    constexpr std::string_view Paper = "*Paper_Space";

    template <class DrawingT>
    auto Named(DrawingT &drawing, std::string_view name) -> auto *
    {
      const auto found =
          std::ranges::find_if(drawing.Blocks, [name](const Blocks::Definition &one) -> bool
                               { return Matches(one.Block.Name, name); });
      return found == drawing.Blocks.end() ? nullptr : &*found;
    }

    /**
     *  A pointer to a part of a drawing, read only when the drawing is.
     */
    template <class DrawingT, class PartT>
    using Pointer = std::conditional_t<std::is_const_v<DrawingT>, const PartT *, PartT *>;

    template <class DrawingT>
    auto Laid(DrawingT &drawing) -> std::vector<Pointer<DrawingT, Objects::Layout>>
    {
      std::vector<Pointer<DrawingT, Objects::Layout>> out;
      for (auto &one : drawing.Objects)
      {
        if (auto *layout = std::get_if<Objects::Layout>(&one))
        {
          out.push_back(layout);
        }
      }
      std::ranges::stable_sort(out, {},
                               [](const Objects::Layout *one) -> int { return one->Order; });
      return out;
    }

    template <class DrawingT>
    auto Called(DrawingT &drawing, std::string_view name) -> Pointer<DrawingT, Objects::Layout>
    {
      for (auto &one : drawing.Objects)
      {
        auto *layout = std::get_if<Objects::Layout>(&one);
        if (layout != nullptr && Matches(layout->Name, name))
        {
          return layout;
        }
      }
      return nullptr;
    }

    template <class DrawingT>
    auto Rooted(DrawingT &drawing) -> Pointer<DrawingT, Objects::Dictionary>
    {
      for (auto &one : drawing.Objects)
      {
        auto *dictionary = std::get_if<Objects::Dictionary>(&one);
        if (dictionary != nullptr && !dictionary->Base.Owner)
        {
          return dictionary;
        }
      }
      return nullptr;
    }

    template <class DrawingT>
    auto Entered(DrawingT &drawing, const Objects::Dictionary &dictionary, std::string_view name)
        -> Pointer<DrawingT, Object>
    {
      const auto found =
          std::ranges::find_if(dictionary.Entries, [name](const Objects::Entry &one) -> bool
                               { return Matches(one.Name, name); });
      if (found == dictionary.Entries.end())
      {
        return nullptr;
      }
      return FindObject(drawing, found->Hard ? found->Hard : found->Soft);
    }

    template <class DrawingT>
    auto Looked(DrawingT &drawing, std::string_view name) -> Pointer<DrawingT, Objects::Dictionary>
    {
      const Objects::Dictionary *root  = Rooted(drawing);
      auto                      *found = root == nullptr ? nullptr : Entered(drawing, *root, name);
      return found == nullptr ? nullptr : std::get_if<Objects::Dictionary>(found);
    }
  } // namespace

  auto Matches(std::string_view one, std::string_view other) -> bool
  {
    return Ascii::Same(one, other);
  }

  auto Block(Drawing &drawing, std::string_view name) -> Blocks::Definition *
  {
    return Named(drawing, name);
  }

  auto Block(const Drawing &drawing, std::string_view name) -> const Blocks::Definition *
  {
    return Named(drawing, name);
  }

  auto ModelSpace(Drawing &drawing) -> Tables::BlockRecord *
  {
    return Record<Tables::BlockRecord>(drawing, Model);
  }

  auto ModelSpace(const Drawing &drawing) -> const Tables::BlockRecord *
  {
    return Record<Tables::BlockRecord>(drawing, Model);
  }

  auto PaperSpace(Drawing &drawing) -> Tables::BlockRecord *
  {
    return Record<Tables::BlockRecord>(drawing, Paper);
  }

  auto PaperSpace(const Drawing &drawing) -> const Tables::BlockRecord *
  {
    return Record<Tables::BlockRecord>(drawing, Paper);
  }

  auto Layouts(Drawing &drawing) -> std::vector<Objects::Layout *>
  {
    return Laid(drawing);
  }

  auto Layouts(const Drawing &drawing) -> std::vector<const Objects::Layout *>
  {
    return Laid(drawing);
  }

  auto Layout(Drawing &drawing, std::string_view name) -> Objects::Layout *
  {
    return Called(drawing, name);
  }

  auto Layout(const Drawing &drawing, std::string_view name) -> const Objects::Layout *
  {
    return Called(drawing, name);
  }

  auto Root(Drawing &drawing) -> Objects::Dictionary *
  {
    return Rooted(drawing);
  }

  auto Root(const Drawing &drawing) -> const Objects::Dictionary *
  {
    return Rooted(drawing);
  }

  auto Entry(Drawing &drawing, const Objects::Dictionary &dictionary, std::string_view name)
      -> Object *
  {
    return Entered(drawing, dictionary, name);
  }

  auto Entry(const Drawing &drawing, const Objects::Dictionary &dictionary, std::string_view name)
      -> const Object *
  {
    return Entered(drawing, dictionary, name);
  }

  auto Dictionary(Drawing &drawing, std::string_view name) -> Objects::Dictionary *
  {
    return Looked(drawing, name);
  }

  auto Dictionary(const Drawing &drawing, std::string_view name) -> const Objects::Dictionary *
  {
    return Looked(drawing, name);
  }
} // namespace DxfIO::View
