#ifndef DXFIO_ENTITIES_SHAPES_RASTER_HPP
#define DXFIO_ENTITIES_SHAPES_RASTER_HPP

#include "fields/count.hpp"
#include "fields/every.hpp"
#include "fields/field.hpp"
#include "fields/flat.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/raster.hpp"
#include "dxfio/version.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Pixel> = std::tuple{
    Fields::Flat{.Code = 14, .Member = &Entities::Pixel::Position},
};

template <>
inline constexpr auto Shapes<Entities::Raster> = std::tuple{
    Fields::Field{.Code = 90, .Member = &Entities::Raster::Version},
    Fields::Field{.Code = 10, .Member = &Entities::Raster::Origin},
    Fields::Field{.Code = 11, .Member = &Entities::Raster::Across},
    Fields::Field{.Code = 12, .Member = &Entities::Raster::Down},
    Fields::Flat{.Code = 13, .Member = &Entities::Raster::Size},
    Fields::Field{.Code = 340, .Member = &Entities::Raster::Definition},
    Fields::Field{.Code = 70, .Member = &Entities::Raster::Flags},
    Fields::Field{.Code = 280, .Member = &Entities::Raster::Clipped},
    Fields::Field{.Code = 281, .Member = &Entities::Raster::Brightness},
    Fields::Field{.Code = 282, .Member = &Entities::Raster::Contrast},
    Fields::Field{.Code = 283, .Member = &Entities::Raster::Fade},
    Fields::Since{
        .Release = Version::R2010,
        .Fields = std::tuple{Fields::Field{
            .Code = 290,
            .Member = &Entities::Raster::Outside,
        }},
    },
    Fields::Field{.Code = 360, .Member = &Entities::Raster::Reactor},
    Fields::Field{.Code = 71, .Member = &Entities::Raster::Kind},
    Fields::Count{.Code = 91, .Member = &Entities::Raster::Corners},
    Fields::Every{
        .Opener = 14,
        .Member = &Entities::Raster::Corners,
        .Fields = Shapes<Entities::Pixel>,
    },
};

template <>
inline constexpr auto Shapes<Entities::Image> = std::tuple{
    Entities::Part<Entities::Image>,
    Fields::Picture{.Member = &Entities::Image::Picture},
    Fields::Subclass{.Marker = "AcDbRasterImage"},
    Fields::Part{
        .Member = &Entities::Image::Raster,
        .Fields = Shapes<Entities::Raster>,
    },
};

template <>
inline constexpr auto Shapes<Entities::Wipeout> = std::tuple{
    Entities::Part<Entities::Wipeout>,
    Fields::Picture{.Member = &Entities::Wipeout::Picture},
    Fields::Subclass{.Marker = "AcDbWipeout"},
    Fields::Part{
        .Member = &Entities::Wipeout::Raster,
        .Fields = Shapes<Entities::Raster>,
    },
};

template <>
inline constexpr std::string_view Type<Entities::Image> = "IMAGE";

template <>
inline constexpr std::string_view Type<Entities::Wipeout> = "WIPEOUT";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_RASTER_HPP */
