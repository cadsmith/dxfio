#ifndef DXFIO_OBJECTS_SHAPES_SETTINGS_HPP
#define DXFIO_OBJECTS_SHAPES_SETTINGS_HPP

#include "fields/part.hpp"
#include "plot.hpp"
#include "shapes.hpp"

#include "dxfio/objects/plotsettings.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Objects::PlotSettings> = std::tuple{
    Fields::Part{
        .Member = &Objects::PlotSettings::Plot,
        .Fields = Shapes<Objects::Plot>,
    },
};

template <>
inline constexpr std::string_view Type<Objects::PlotSettings> = "PLOTSETTINGS";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_SETTINGS_HPP */
