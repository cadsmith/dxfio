#ifndef DXFIO_HEADER_ROWS_AXES_HPP
#define DXFIO_HEADER_ROWS_AXES_HPP

#include "header/row.hpp"

#include <array>

namespace DxfIO::Header::Rows
{
  inline constexpr std::array Axes{
      Row{.Name = "$UCSBASE", .Code = 2, .Value = "", .Since = Version::R2000},
      Row{.Name = "$UCSNAME", .Code = 2, .Value = ""},
      Row{
          .Name  = "$UCSORG",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
      },
      Row{
          .Name  = "$UCSXDIR",
          .Code  = 10,
          .Value = Vector{.X = 1.0, .Y = 0.0, .Z = 0.0},
      },
      Row{
          .Name  = "$UCSYDIR",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 1.0, .Z = 0.0},
      },
      Row{
          .Name  = "$UCSORTHOREF",
          .Code  = 2,
          .Value = "",
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$UCSORTHOVIEW",
          .Code  = 70,
          .Value =  0,
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$UCSORGTOP",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$UCSORGBOTTOM",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$UCSORGLEFT",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$UCSORGRIGHT",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$UCSORGFRONT",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$UCSORGBACK",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{.Name = "$PUCSBASE", .Code = 2, .Value = "", .Since = Version::R2000},
      Row{.Name = "$PUCSNAME", .Code = 2, .Value = ""},
      Row{
          .Name  = "$PUCSORG",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
      },
      Row{
          .Name  = "$PUCSXDIR",
          .Code  = 10,
          .Value = Vector{.X = 1.0, .Y = 0.0, .Z = 0.0},
      },
      Row{
          .Name  = "$PUCSYDIR",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 1.0, .Z = 0.0},
      },
      Row{
          .Name  = "$PUCSORTHOREF",
          .Code  = 2,
          .Value = "",
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$PUCSORTHOVIEW",
          .Code  = 70,
          .Value =  0,
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$PUCSORGTOP",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$PUCSORGBOTTOM",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$PUCSORGLEFT",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$PUCSORGRIGHT",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$PUCSORGFRONT",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$PUCSORGBACK",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
  };
} // namespace DxfIO::Header::Rows

#endif /* DXFIO_HEADER_ROWS_AXES_HPP */
