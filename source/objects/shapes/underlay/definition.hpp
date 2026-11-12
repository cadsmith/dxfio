#ifndef DXFIO_OBJECTS_SHAPES_UNDERLAY_DEFINITION_HPP
#define DXFIO_OBJECTS_SHAPES_UNDERLAY_DEFINITION_HPP

#include "fields/field.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/objects/underlay/definition.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <Entities::Overlay KindV>
  inline constexpr auto Shapes<Objects::UnderlayDefinition<KindV>> = std::tuple{
      Fields::Subclass{.Marker = "AcDbUnderlayDefinition"},
      Fields::Field{
          .Code   = 1,
          .Member = &Objects::UnderlayDefinition<KindV>::File,
      },
      Fields::Field{
          .Code   = 2,
          .Member = &Objects::UnderlayDefinition<KindV>::Name,
      },
  };

  template <>
  inline constexpr std::string_view Type<Objects::PdfDefinition> = "PDFDEFINITION";

  template <>
  inline constexpr std::string_view Type<Objects::DwfDefinition> = "DWFDEFINITION";

  template <>
  inline constexpr std::string_view Type<Objects::DgnDefinition> = "DGNDEFINITION";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_UNDERLAY_DEFINITION_HPP */
