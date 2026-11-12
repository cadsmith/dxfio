#ifndef DXFIO_ENTITIES_SHAPES_TRACE_HPP
#define DXFIO_ENTITIES_SHAPES_TRACE_HPP

#include "filled.hpp"
#include "shapes.hpp"

#include "dxfio/entities/trace.hpp"

#include <string_view>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Trace> = Entities::Filled<Entities::Trace>;

  template <>
  inline constexpr std::string_view Type<Entities::Trace> = "TRACE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_TRACE_HPP */
