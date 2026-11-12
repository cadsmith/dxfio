#ifndef DXFIO_HEADER_ROWS_DRAFTING_HPP
#define DXFIO_HEADER_ROWS_DRAFTING_HPP

#include "header/row.hpp"

#include <array>

namespace DxfIO::Header::Rows
{
  inline constexpr std::array Drafting{
      Row{.Name = "$LUNITS", .Code = 70, .Value = 2},
      Row{.Name = "$LUPREC", .Code = 70, .Value = 4},
      Row{.Name = "$SKETCHINC", .Code = 40, .Value = 1.0},
      Row{.Name = "$FILLETRAD", .Code = 40, .Value = 0.0},
      Row{.Name = "$AUNITS", .Code = 70, .Value = 0},
      Row{.Name = "$AUPREC", .Code = 70, .Value = 0},
      Row{.Name = "$MENU", .Code = 1, .Value = "."},
      Row{.Name = "$ELEVATION", .Code = 40, .Value = 0.0},
      Row{.Name = "$PELEVATION", .Code = 40, .Value = 0.0},
      Row{.Name = "$THICKNESS", .Code = 40, .Value = 0.0},
      Row{.Name = "$LIMCHECK", .Code = 70, .Value = 0},
      Row{.Name = "$BLIPMODE", .Code = 70, .Value = 0, .Until = Version::R12},
      Row{.Name = "$CHAMFERA", .Code = 40, .Value = 0.0},
      Row{.Name = "$CHAMFERB", .Code = 40, .Value = 0.0},
      Row{.Name = "$CHAMFERC", .Code = 40, .Value = 0.0, .Since = Version::R2000},
      Row{.Name = "$CHAMFERD", .Code = 40, .Value = 0.0, .Since = Version::R2000},
      Row{.Name = "$SKPOLY", .Code = 70, .Value = 0},
      Row{.Name = "$TDCREATE", .Code = 40, .Value = 0.0},
      Row{
          .Name  = "$TDUCREATE",
          .Code  = 40,
          .Value = 0.0,
          .Since = Version::R2000,
      },
      Row{.Name = "$TDUPDATE", .Code = 40, .Value = 0.0},
      Row{
          .Name  = "$TDUUPDATE",
          .Code  = 40,
          .Value = 0.0,
          .Since = Version::R2000,
      },
      Row{.Name = "$TDINDWG", .Code = 40, .Value = 0.0},
      Row{.Name = "$TDUSRTIMER", .Code = 40, .Value = 0.0},
      Row{.Name = "$USRTIMER", .Code = 70, .Value = 1},
      Row{.Name = "$ANGBASE", .Code = 50, .Value = 0.0},
      Row{.Name = "$ANGDIR", .Code = 70, .Value = 0},
      Row{.Name = "$PDMODE", .Code = 70, .Value = 0},
      Row{.Name = "$PDSIZE", .Code = 40, .Value = 0.0},
      Row{.Name = "$PLINEWID", .Code = 40, .Value = 0.0},
      Row{.Name = "$COORDS", .Code = 70, .Value = 1, .Until = Version::R12},
      Row{.Name = "$SPLFRAME", .Code = 70, .Value = 0},
      Row{.Name = "$SPLINETYPE", .Code = 70, .Value = 6},
      Row{.Name = "$SPLINESEGS", .Code = 70, .Value = 8},
      Row{.Name = "$ATTDIA", .Code = 70, .Value = 1, .Until = Version::R12},
      Row{.Name = "$ATTREQ", .Code = 70, .Value = 1, .Until = Version::R12},
      Row{.Name = "$HANDLING", .Code = 70, .Value = 1, .Until = Version::R12},
      Row{.Name = "$HANDSEED", .Code = 5, .Value = Handle{}},
      Row{.Name = "$SURFTAB1", .Code = 70, .Value = 6},
      Row{.Name = "$SURFTAB2", .Code = 70, .Value = 6},
      Row{.Name = "$SURFTYPE", .Code = 70, .Value = 6},
      Row{.Name = "$SURFU"   , .Code = 70, .Value = 6},
      Row{.Name = "$SURFV"   , .Code = 70, .Value = 6},
  };
} // namespace DxfIO::Header::Rows

#endif /* DXFIO_HEADER_ROWS_DRAFTING_HPP */
