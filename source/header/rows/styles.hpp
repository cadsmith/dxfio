#ifndef DXFIO_HEADER_ROWS_STYLES_HPP
#define DXFIO_HEADER_ROWS_STYLES_HPP

#include "header/row.hpp"

#include <array>

namespace DxfIO::Header::Rows
{
  inline constexpr std::array Styles{
      Row{
          .Name  = "$CMLSTYLE",
          .Code  = 2,
          .Value = "Standard",
          .Since = Version::R2000,
      },
      Row{.Name = "$CMLJUST", .Code = 70, .Value = 0, .Since = Version::R2000},
      Row{
          .Name  = "$CMLSCALE",
          .Code  = 40,
          .Value = 20.0,
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$PROXYGRAPHICS",
          .Code  = 70,
          .Value =  1,
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$MEASUREMENT",
          .Code  = 70,
          .Value =  1,
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$CELWEIGHT",
          .Code  = 370,
          .Value =  -1,
          .Since = Version::R2000,
      },
      Row{.Name = "$ENDCAPS"  , .Code = 280, .Value = 0, .Since = Version::R2000},
      Row{.Name = "$JOINSTYLE", .Code = 280, .Value = 0, .Since = Version::R2000},
      Row{
          .Name  = "$LWDISPLAY",
          .Code  = 290,
          .Value = false,
          .Since = Version::R2000,
      },
      Row{.Name = "$INSUNITS", .Code = 70, .Value = 4, .Since = Version::R2000},
      Row{
          .Name  = "$HYPERLINKBASE",
          .Code  = 1,
          .Value = "",
          .Since = Version::R2000,
      },
      Row{.Name = "$STYLESHEET", .Code =   1, .Value = ""  , .Since = Version::R2000},
      Row{.Name = "$XEDIT"     , .Code = 290, .Value = true, .Since = Version::R2000},
      Row{.Name = "$CEPSNTYPE" , .Code = 380, .Value = 0   , .Since = Version::R2000},
      Row{
          .Name  = "$PSTYLEMODE",
          .Code  = 290,
          .Value = true,
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$FINGERPRINTGUID",
          .Code  = 2,
          .Value = "",
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$VERSIONGUID",
          .Code  = 2,
          .Value = "",
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$EXTNAMES",
          .Code  = 290,
          .Value = true,
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$PSVPSCALE",
          .Code  = 40,
          .Value = 0.0,
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$OLESTARTUP",
          .Code  = 290,
          .Value = false,
          .Since = Version::R2000,
      },
      Row{
          .Name  = "$SORTENTS",
          .Code  = 280,
          .Value = 127,
          .Since = Version::R2004,
      },
      Row{.Name = "$INDEXCTL", .Code = 280, .Value = 0, .Since = Version::R2004},
      Row{.Name = "$HIDETEXT", .Code = 280, .Value = 1, .Since = Version::R2004},
      Row{
          .Name  = "$XCLIPFRAME",
          .Code  = 290,
          .Value = false,
          .Since = Version::R2004,
          .Until = Version::R2007,
      },
      Row{
          .Name  = "$XCLIPFRAME",
          .Code  = 280,
          .Value =   2,
          .Since = Version::R2010,
      },
      Row{.Name = "$HALOGAP" , .Code = 280, .Value =   0, .Since = Version::R2004},
      Row{.Name = "$OBSCOLOR", .Code =  70, .Value = 257, .Since = Version::R2004},
      Row{.Name = "$OBSLTYPE", .Code = 280, .Value =   0, .Since = Version::R2004},
      Row{
          .Name  = "$INTERSECTIONDISPLAY",
          .Code  = 280,
          .Value =   0,
          .Since = Version::R2004,
      },
      Row{
          .Name  = "$INTERSECTIONCOLOR",
          .Code  =  70,
          .Value = 257,
          .Since = Version::R2004,
      },
      Row{.Name = "$DIMASSOC", .Code = 280, .Value = 2, .Since = Version::R2004},
  };
} // namespace DxfIO::Header::Rows

#endif /* DXFIO_HEADER_ROWS_STYLES_HPP */
