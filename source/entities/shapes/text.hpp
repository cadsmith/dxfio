#ifndef DXFIO_ENTITIES_SHAPES_TEXT_HPP
#define DXFIO_ENTITIES_SHAPES_TEXT_HPP

#include "fields/field.hpp"
#include "fields/joined.hpp"
#include "fields/numbers.hpp"
#include "lettering.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/text.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Text> = std::tuple{
      Entities::Part<Entities::Text>,
      Entities::Lettered<Entities::Text>,
      Fields::Joined{.Marker = "AcDbText"},
      Fields::Field{
          .Code     = 73,
          .Member   = &Entities::Text::Vertical,
          .Fallback = 0,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Text> = "TEXT";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_TEXT_HPP */
