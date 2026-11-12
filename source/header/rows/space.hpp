#ifndef DXFIO_HEADER_ROWS_SPACE_HPP
#define DXFIO_HEADER_ROWS_SPACE_HPP

#include "header/row.hpp"

#include <array>

namespace DxfIO::Header::Rows
{
  inline constexpr std::array Space{
      Row{.Name = "$USERI1"   , .Code = 70, .Value =   0},
      Row{.Name = "$USERI2"   , .Code = 70, .Value =   0},
      Row{.Name = "$USERI3"   , .Code = 70, .Value =   0},
      Row{.Name = "$USERI4"   , .Code = 70, .Value =   0},
      Row{.Name = "$USERI5"   , .Code = 70, .Value =   0},
      Row{.Name = "$USERR1"   , .Code = 40, .Value = 0.0},
      Row{.Name = "$USERR2"   , .Code = 40, .Value = 0.0},
      Row{.Name = "$USERR3"   , .Code = 40, .Value = 0.0},
      Row{.Name = "$USERR4"   , .Code = 40, .Value = 0.0},
      Row{.Name = "$USERR5"   , .Code = 40, .Value = 0.0},
      Row{.Name = "$WORLDVIEW", .Code = 70, .Value =   1},
      Row{.Name = "$SHADEDGE" , .Code = 70, .Value =   3},
      Row{.Name = "$SHADEDIF" , .Code = 70, .Value =  70},
      Row{.Name = "$TILEMODE" , .Code = 70, .Value =   1},
      Row{.Name = "$MAXACTVP" , .Code = 70, .Value =  64},
      Row{
          .Name  = "$PINSBASE",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Since = Version::R2000,
      },
      Row{.Name = "$PLIMCHECK", .Code = 70, .Value = 0},
      Row{
          .Name  = "$PEXTMIN",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
      },
      Row{
          .Name  = "$PEXTMAX",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
      },
      Row{
          .Name  = "$PLIMMIN",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Flat  = true,
      },
      Row{
          .Name  = "$PLIMMAX",
          .Code  = 10,
          .Value = Vector{.X = 12.0, .Y = 9.0, .Z = 0.0},
          .Flat  = true,
      },
      Row{.Name = "$UNITMODE" , .Code = 70, .Value = 0},
      Row{.Name = "$VISRETAIN", .Code = 70, .Value = 1},
      Row{.Name = "$PLINEGEN" , .Code = 70, .Value = 0},
      Row{.Name = "$PSLTSCALE", .Code = 70, .Value = 1},
      Row{
          .Name  = "$TREEDEPTH",
          .Code  =   70,
          .Value = 3020,
          .Since = Version::R2000,
      },
  };
} // namespace DxfIO::Header::Rows

#endif /* DXFIO_HEADER_ROWS_SPACE_HPP */
