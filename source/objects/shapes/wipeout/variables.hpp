#ifndef DXFIO_OBJECTS_SHAPES_WIPEOUT_VARIABLES_HPP
#define DXFIO_OBJECTS_SHAPES_WIPEOUT_VARIABLES_HPP

#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"
#include "shapes.hpp"

#include "dxfio/objects/wipeout/variables.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Objects::WipeoutVariables> = std::tuple{
      Fields::Subclass{.Marker = "AcDbWipeoutVariables"},
      Fields::Field{.Code = 70, .Member = &Objects::WipeoutVariables::Frame},
  };

  template <>
  inline constexpr std::string_view Type<Objects::WipeoutVariables> = "WIPEOUTVARIABLES";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_WIPEOUT_VARIABLES_HPP */
