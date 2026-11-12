#ifndef DXFIO_TABLES_SHAPES_UCS_HPP
#define DXFIO_TABLES_SHAPES_UCS_HPP

#include "fields/handles.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "record.hpp"

#include "dxfio/tables/ucs.hpp"

#include <string_view>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Tables::Ucs> = std::tuple{
      Tables::Opening,
      Fields::Subclass{.Marker = "AcDbUCSTableRecord"},
      Fields::Part{
          .Member = &Tables::Ucs::Record,
          .Fields = Shapes<Tables::Record>,
      },
      Fields::Field{.Code = 10, .Member = &Tables::Ucs::Origin},
      Fields::Field{.Code = 11, .Member = &Tables::Ucs::X},
      Fields::Field{.Code = 12, .Member = &Tables::Ucs::Y},
      Fields::Field{
          .Code   = 79,
          .Member = &Tables::Ucs::Ortho,
          .Since  = Version::R2000,
      },
      Fields::Field{
          .Code   = 146,
          .Member = &Tables::Ucs::Elevation,
          .Since  = Version::R2000,
      },
      Fields::Field{
          .Code     = 346,
          .Member   = &Tables::Ucs::Basis,
          .Fallback = Handle{},
          .Since    = Version::R2000,
      },
  };

  template <>
  inline constexpr std::string_view Type<Tables::Ucs> = "UCS";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_UCS_HPP */
