#ifndef DXFIO_ENTITIES_SHAPES_TABLE_HPP
#define DXFIO_ENTITIES_SHAPES_TABLE_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "insert.hpp"
#include "record.hpp"
#include "shapes.hpp"
#include "trail.hpp"

#include "dxfio/entities/table.hpp"
#include "dxfio/vector.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::AcadTable> = std::tuple{
      Entities::Part<Entities::AcadTable>,
      Fields::Picture{.Member = &Entities::AcadTable::Picture},
      Fields::Subclass{.Marker = "AcDbBlockReference"},
      Fields::Field{.Code = 2, .Member = &Entities::AcadTable::Block},
      Fields::Field{.Code = 10, .Member = &Entities::AcadTable::Position},
      Fields::Part{.Member = &Entities::AcadTable::Scale, .Fields = Entities::Scales},
      Fields::Field{
          .Code     = 50,
          .Member   = &Entities::AcadTable::Rotation,
          .Fallback = 0.0,
      },
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::AcadTable::Extrusion,
          .Fallback = Up,
      },
      Entities::Trailed<Entities::AcadTable, &Entities::AcadTable::Body>,
  };

  template <>
  inline constexpr std::string_view Type<Entities::AcadTable> = "ACAD_TABLE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_TABLE_HPP */
