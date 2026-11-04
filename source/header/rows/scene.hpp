#ifndef DXFIO_HEADER_ROWS_SCENE_HPP
#define DXFIO_HEADER_ROWS_SCENE_HPP

#include "header/row.hpp"

#include <array>

namespace DxfIO::Header::Rows
{
inline constexpr std::array Scene{
    Row{
        .Name = "$PROJECTNAME",
        .Code = 1,
        .Value = "",
        .Since = Version::R2004,
    },
    Row{
        .Name = "$CAMERADISPLAY",
        .Code = 290,
        .Value = false,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$LENSLENGTH",
        .Code = 40,
        .Value = 50.0,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$CAMERAHEIGHT",
        .Code = 40,
        .Value = 0.0,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$STEPSPERSEC",
        .Code = 40,
        .Value = 2.0,
        .Since = Version::R2007,
    },
    Row{.Name = "$STEPSIZE", .Code = 40, .Value = 6.0, .Since = Version::R2007},
    Row{
        .Name = "$3DDWFPREC",
        .Code = 40,
        .Value = 2.0,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$PSOLWIDTH",
        .Code = 40,
        .Value = 5.0,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$PSOLHEIGHT",
        .Code = 40,
        .Value = 80.0,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$LOFTANG1",
        .Code = 40,
        .Value = 1.570796326794896,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$LOFTANG2",
        .Code = 40,
        .Value = 1.570796326794896,
        .Since = Version::R2007,
    },
    Row{.Name = "$LOFTMAG1", .Code = 40, .Value = 0.0, .Since = Version::R2007},
    Row{.Name = "$LOFTMAG2", .Code = 40, .Value = 0.0, .Since = Version::R2007},
    Row{.Name = "$LOFTPARAM", .Code = 70, .Value = 7, .Since = Version::R2007},
    Row{
        .Name = "$LOFTNORMALS",
        .Code = 280,
        .Value = 1,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$LATITUDE",
        .Code = 40,
        .Value = 37.795,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$LONGITUDE",
        .Code = 40,
        .Value = -122.394,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$NORTHDIRECTION",
        .Code = 40,
        .Value = 0.0,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$TIMEZONE",
        .Code = 70,
        .Value = -8000,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$LIGHTGLYPHDISPLAY",
        .Code = 280,
        .Value = 1,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$TILEMODELIGHTSYNCH",
        .Code = 280,
        .Value = 1,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$CMATERIAL",
        .Code = 347,
        .Value = Handle{.Value = 0xEC},
        .Since = Version::R2007,
    },
    Row{.Name = "$SOLIDHIST", .Code = 280, .Value = 0, .Since = Version::R2007},
    Row{.Name = "$SHOWHIST", .Code = 280, .Value = 1, .Since = Version::R2007},
    Row{.Name = "$DWFFRAME", .Code = 280, .Value = 2, .Since = Version::R2007},
    Row{.Name = "$DGNFRAME", .Code = 280, .Value = 0, .Since = Version::R2007},
    Row{
        .Name = "$REALWORLDSCALE",
        .Code = 290,
        .Value = true,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$INTERFERECOLOR",
        .Code = 62,
        .Value = 1,
        .Since = Version::R2007,
    },
    Row{
        .Name = "$INTERFEREOBJVS",
        .Code = 345,
        .Value = Handle{.Value = 0xF9},
        .Since = Version::R2007,
    },
    Row{
        .Name = "$INTERFEREVPVS",
        .Code = 346,
        .Value = Handle{.Value = 0xF6},
        .Since = Version::R2007,
    },
    Row{.Name = "$CSHADOW", .Code = 280, .Value = 0, .Since = Version::R2007},
    Row{
        .Name = "$SHADOWPLANELOCATION",
        .Code = 40,
        .Value = 0.0,
        .Since = Version::R2007,
    },
};
} // namespace DxfIO::Header::Rows

#endif /* DXFIO_HEADER_ROWS_SCENE_HPP */
