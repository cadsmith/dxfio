#ifndef DXFIO_ENTITIES_SHAPES_ATTRIB_HPP
#define DXFIO_ENTITIES_SHAPES_ATTRIB_HPP

#include "attribute.hpp"
#include "fields/subclass.hpp"
#include "lettering.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/attrib.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Attrib> = std::tuple_cat(
      std::tuple{
          Entities::Part<Entities::Attrib>,
          Entities::Lettered<Entities::Attrib>,
          Fields::Subclass{.Marker = "AcDbAttribute"},
          Entities::Revised<Entities::Attrib>,
      },
      Entities::Labeled<Entities::Attrib>);

  template <>
  inline constexpr std::string_view Type<Entities::Attrib> = "ATTRIB";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_ATTRIB_HPP */
