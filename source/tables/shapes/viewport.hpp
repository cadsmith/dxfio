#ifndef DXFIO_TABLES_SHAPES_VIEWPORT_HPP
#define DXFIO_TABLES_SHAPES_VIEWPORT_HPP

#include "axes.hpp"
#include "fields/flat.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "record.hpp"

#include "dxfio/tables/vport.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Tables
  {
    inline constexpr auto Frame = std::tuple{
        Fields::Flat{.Code = 10, .Member = &VPort::Lower},
        Fields::Flat{.Code = 11, .Member = &VPort::Upper},
        Fields::Flat{.Code = 12, .Member = &VPort::Center},
        Fields::Flat{.Code = 13, .Member = &VPort::Base},
        Fields::Flat{.Code = 14, .Member = &VPort::Snap},
        Fields::Flat{.Code = 15, .Member = &VPort::Grid},
        Fields::Field{.Code = 16, .Member = &VPort::Direction},
        Fields::Field{.Code = 17, .Member = &VPort::Target},
        Fields::Field{.Code = 40, .Member = &VPort::Height},
        Fields::Field{.Code = 41, .Member = &VPort::Aspect},
        Fields::Field{.Code = 42, .Member = &VPort::Focal},
        Fields::Field{.Code = 43, .Member = &VPort::Front},
        Fields::Field{.Code = 44, .Member = &VPort::Back},
        Fields::Field{.Code = 50, .Member = &VPort::Rotation},
        Fields::Field{.Code = 51, .Member = &VPort::Twist},
    };

    inline constexpr auto Modes = std::tuple{
        Fields::Field{.Code = 71, .Member = &VPort::Mode},
        Fields::Field{.Code = 72, .Member = &VPort::Zoom},
        Fields::Field{.Code = 73, .Member = &VPort::Fast},
        Fields::Field{.Code = 74, .Member = &VPort::Icon},
        Fields::Field{.Code = 75, .Member = &VPort::Snapped},
        Fields::Field{.Code = 76, .Member = &VPort::Gridded},
        Fields::Field{.Code = 77, .Member = &VPort::Isometric},
        Fields::Field{.Code = 78, .Member = &VPort::Isoplane},
    };

    inline constexpr auto Oriented = std::tuple{
        Fields::Field{.Code = 281, .Member = &VPort::Render},
        Fields::Field{.Code = 65, .Member = &VPort::Ucsvp},
        Fields::Part{.Member = &VPort::Axes, .Fields = Shapes<Axes>},
    };

    inline constexpr auto Lights = std::tuple{
        Fields::Field{.Code = 348, .Member = &VPort::Style},
        Fields::Field{.Code = 60, .Member = &VPort::Display},
        Fields::Field{.Code = 61, .Member = &VPort::Major},
        Fields::Field{.Code = 332, .Member = &VPort::Background, .Fallback = Handle{}},
        Fields::Field{.Code = 333, .Member = &VPort::Shadow, .Fallback = Handle{}},
        Fields::Field{.Code = 292, .Member = &VPort::Lit},
        Fields::Field{.Code = 282, .Member = &VPort::Lighting},
        Fields::Field{.Code = 141, .Member = &VPort::Brightness},
        Fields::Field{.Code = 142, .Member = &VPort::Contrast},
        Fields::Field{.Code = 63, .Member = &VPort::Ambient},
        Fields::Field{.Code = 421, .Member = &VPort::Rgb},
    };
  } // namespace Tables

  template <>
  inline constexpr auto Shapes<Tables::VPort> = std::tuple_cat(
      std::tuple{
          Tables::Opening,
          Fields::Subclass{.Marker = "AcDbViewportTableRecord"},
          Fields::Part{
              .Member = &Tables::VPort::Record,
              .Fields = Shapes<Tables::Record>,
          },
      },
      Tables::Frame, Tables::Modes,
      std::tuple{
          Fields::Since{.Release = Version::R2000, .Fields = Tables::Oriented},
          Fields::Since{.Release = Version::R2007, .Fields = Tables::Lights},
      });

  template <>
  inline constexpr std::string_view Type<Tables::VPort> = "VPORT";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_VIEWPORT_HPP */
