#ifndef DXFIO_HEADER_ROWS_DRAWING_HPP
#define DXFIO_HEADER_ROWS_DRAWING_HPP

#include "header/row.hpp"

#include <array>

namespace DxfIO::Header::Rows
{
  inline constexpr std::array Drawing{
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1006",
          .Until = Version::R10,
      },
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1009",
          .Since = Version::R12,
          .Until = Version::R12,
      },
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1012",
          .Since = Version::R13,
          .Until = Version::R13,
      },
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1014",
          .Since = Version::R14,
          .Until = Version::R14,
      },
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1015",
          .Since = Version::R2000,
          .Until = Version::R2000,
      },
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1018",
          .Since = Version::R2004,
          .Until = Version::R2004,
      },
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1021",
          .Since = Version::R2007,
          .Until = Version::R2007,
      },
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1024",
          .Since = Version::R2010,
          .Until = Version::R2010,
      },
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1027",
          .Since = Version::R2013,
          .Until = Version::R2013,
      },
      Row{
          .Name  = "$ACADVER",
          .Code  = 1,
          .Value = "AC1032",
          .Since = Version::R2018,
      },
      Row{
          .Name  = "$ACADMAINTVER",
          .Code  = 70,
          .Value = 20,
          .Since = Version::R2000,
          .Until = Version::R2000,
      },
      Row{
          .Name  = "$ACADMAINTVER",
          .Code  =  70,
          .Value = 104,
          .Since = Version::R2004,
          .Until = Version::R2004,
      },
      Row{
          .Name  = "$ACADMAINTVER",
          .Code  = 70,
          .Value = 50,
          .Since = Version::R2007,
          .Until = Version::R2010,
      },
      Row{
          .Name  = "$ACADMAINTVER",
          .Code  =  70,
          .Value = 125,
          .Since = Version::R2013,
          .Until = Version::R2013,
      },
      Row{
          .Name  = "$ACADMAINTVER",
          .Code  =  90,
          .Value = 377,
          .Since = Version::R2018,
      },
      Row{
          .Name  = "$DWGCODEPAGE",
          .Code  = 3,
          .Value = "ANSI_1252",
          .Since = Version::R13,
      },
      Row{
          .Name  = "$LASTSAVEDBY",
          .Code  = 1,
          .Value = "",
          .Since = Version::R2004,
      },
      Row{
          .Name  = "$REQUIREDVERSIONS",
          .Code  = 160,
          .Value =   0,
          .Since = Version::R2013,
      },
      Row{
          .Name  = "$INSBASE",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
      },
      Row{
          .Name  = "$EXTMIN",
          .Code  = 10,
          .Value = Vector{.X = 1e20, .Y = 1e20, .Z = 1e20},
      },
      Row{
          .Name  = "$EXTMAX",
          .Code  = 10,
          .Value = Vector{.X = -1e20, .Y = -1e20, .Z = -1e20},
      },
      Row{
          .Name  = "$LIMMIN",
          .Code  = 10,
          .Value = Vector{.X = 0.0, .Y = 0.0, .Z = 0.0},
          .Flat  = true,
      },
      Row{
          .Name  = "$LIMMAX",
          .Code  = 10,
          .Value = Vector{.X = 420.0, .Y = 297.0, .Z = 0.0},
          .Flat  = true,
      },
      Row{.Name = "$ORTHOMODE", .Code = 70, .Value = 0},
      Row{.Name = "$REGENMODE", .Code = 70, .Value = 1},
      Row{.Name = "$FILLMODE", .Code = 70, .Value = 1},
      Row{.Name = "$QTEXTMODE", .Code = 70, .Value = 0},
      Row{.Name = "$MIRRTEXT", .Code = 70, .Value = 0},
      Row{.Name = "$DRAGMODE", .Code = 70, .Value = 2, .Until = Version::R12},
      Row{.Name = "$LTSCALE", .Code = 40, .Value = 1.0},
      Row{.Name = "$OSMODE", .Code = 70, .Value = 37, .Until = Version::R12},
      Row{.Name = "$ATTMODE", .Code = 70, .Value = 1},
      Row{.Name = "$TEXTSIZE", .Code = 40, .Value = 2.5},
      Row{.Name = "$TRACEWID", .Code = 40, .Value = 1.0},
      Row{.Name = "$TEXTSTYLE", .Code = 7, .Value = "Standard"},
      Row{.Name = "$CLAYER", .Code = 8, .Value = "0"},
      Row{.Name = "$CELTYPE", .Code = 6, .Value = "ByLayer"},
      Row{.Name = "$CECOLOR", .Code = 62, .Value = 256},
      Row{
          .Name  = "$CELTSCALE",
          .Code  = 40,
          .Value = 1.0,
          .Since = Version::R2000,
      },
      Row{.Name = "$DISPSILH", .Code = 70, .Value = 0, .Since = Version::R2000},
  };
} // namespace DxfIO::Header::Rows

#endif /* DXFIO_HEADER_ROWS_DRAWING_HPP */
