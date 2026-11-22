#include "dxfio/view.hpp"

#include "ascii.hpp"

#include <algorithm>
#include <variant>

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

  auto ModelSpace(const Drawing &drawing) -> const Tables::BlockRecord *
  {
    return Record<Tables::BlockRecord>(drawing, Model);
  }

  auto PaperSpace(const Drawing &drawing) -> const Tables::BlockRecord *
  {
    return Record<Tables::BlockRecord>(drawing, Paper);
  }

  auto Layouts(const Drawing &drawing) -> std::vector<const Objects::Layout *>
  {
    std::vector<const Objects::Layout *> out;
    for (const Object &one : drawing.Objects)
    {
      if (const auto *layout = std::get_if<Objects::Layout>(&one))
      {
        out.push_back(layout);
      }
    }
    std::ranges::stable_sort(out, {}, [](const Objects::Layout *one) -> int { return one->Order; });
    return out;
  }

  auto Layout(const Drawing &drawing, std::string_view name) -> const Objects::Layout *
  {
    for (const Object &one : drawing.Objects)
    {
      const auto *layout = std::get_if<Objects::Layout>(&one);
      if (layout != nullptr && Matches(layout->Name, name))
      {
        return layout;
      }
    }
    return nullptr;
  }

  auto Root(const Drawing &drawing) -> const Objects::Dictionary *
  {
    for (const Object &one : drawing.Objects)
    {
      const auto *dictionary = std::get_if<Objects::Dictionary>(&one);
      if (dictionary != nullptr && !dictionary->Base.Owner)
      {
        return dictionary;
      }
    }
    return nullptr;
  }

  auto Entry(const Drawing &drawing, const Objects::Dictionary &dictionary, std::string_view name)
      -> const Object *
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

  auto Dictionary(const Drawing &drawing, std::string_view name) -> const Objects::Dictionary *
  {
    const Objects::Dictionary *root  = Root(drawing);
    const Object              *found = root == nullptr ? nullptr : Entry(drawing, *root, name);
    return found == nullptr ? nullptr : std::get_if<Objects::Dictionary>(found);
  }
} // namespace DxfIO::View
