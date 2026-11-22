#include "dxfio/appearance.hpp"

#include "dxfio/color.hpp"
#include "dxfio/view.hpp"

#include <cstdlib>

namespace DxfIO
{
  namespace
  {
    constexpr std::string_view ByLayer = "ByLayer";

    constexpr std::string_view ByBlock = "ByBlock";
  } // namespace

  auto AppearanceOf(const Drawing &drawing, const Entities::Record &record, const Appearance *outer)
      -> Appearance
  {
    Appearance look;
    look.Layer =
        outer != nullptr && View::Matches(record.Layer.View(), "0") ? outer->Layer : record.Layer;
    const Tables::Layer  fallback{};
    const auto          *found = View::Record<Tables::Layer>(drawing, look.Layer.View());
    const Tables::Layer &layer = found == nullptr ? fallback : *found;
    switch (MethodOf(record))
    {
    case ColorMethod::ByLayer:
      /**
       *  A layer turned off is written with its color negated.
       */
      look.Color = std::abs(layer.Color);
      look.Rgb   = layer.Rgb;
      break;
    case ColorMethod::ByBlock:
      look.Color = outer != nullptr ? outer->Color : Colors::White;
      look.Rgb   = outer != nullptr ? outer->Rgb : std::nullopt;
      break;
    default:
      look.Color = record.Color;
      look.Rgb   = record.Rgb;
      break;
    }
    if (View::Matches(record.Linetype.View(), ByLayer))
    {
      look.Linetype = layer.Linetype;
    }
    else if (View::Matches(record.Linetype.View(), ByBlock))
    {
      look.Linetype = outer != nullptr ? outer->Linetype : Symbol{"Continuous"};
    }
    else
    {
      look.Linetype = record.Linetype;
    }
    if (record.Lineweight == Lineweight::ByLayer)
    {
      look.Lineweight = layer.Lineweight;
    }
    else if (record.Lineweight == Lineweight::ByBlock)
    {
      look.Lineweight = outer != nullptr ? outer->Lineweight : Lineweight::Default;
    }
    else
    {
      look.Lineweight = record.Lineweight;
    }
    return look;
  }
} // namespace DxfIO
