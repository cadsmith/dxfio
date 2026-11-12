#ifndef DXFIO_OBJECTS_SHAPES_MLINE_STYLE_HPP
#define DXFIO_OBJECTS_SHAPES_MLINE_STYLE_HPP

#include "fields/count.hpp"
#include "fields/each.hpp"
#include "fields/field.hpp"
#include "fields/first.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/objects/mline/style.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Objects::Mline::Element> = std::tuple{
      Fields::Field{.Code = 49, .Member = &Objects::Mline::Element::Offset},
      Fields::Field{.Code = 62, .Member = &Objects::Mline::Element::Color},
      Fields::Field{.Code = 6, .Member = &Objects::Mline::Element::Linetype},
  };

  template <>
  inline constexpr auto Shapes<Objects::MLineStyle> = std::tuple{
      Fields::Subclass{.Marker = "AcDbMlineStyle"},
      Fields::Field{.Code = 2, .Member = &Objects::MLineStyle::Name},
      Fields::Field{.Code = 70, .Member = &Objects::MLineStyle::Flags},
      Fields::Field{.Code = 3, .Member = &Objects::MLineStyle::Description},
      Fields::First{
          .Code   = 62,
          .Fields = std::tuple{Fields::Field{
              .Code   = 62,
              .Member = &Objects::MLineStyle::Fill,
          }},
      },
      Fields::Field{.Code = 51, .Member = &Objects::MLineStyle::Start},
      Fields::Field{.Code = 52, .Member = &Objects::MLineStyle::End},
      Fields::Count{.Code = 71, .Member = &Objects::MLineStyle::Elements},
      Fields::Each{
          .Opener = 49,
          .Member = &Objects::MLineStyle::Elements,
          .Fields = Shapes<Objects::Mline::Element>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Objects::MLineStyle> = "MLINESTYLE";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_MLINE_STYLE_HPP */
