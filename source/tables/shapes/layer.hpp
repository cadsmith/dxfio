#ifndef DXFIO_TABLES_SHAPES_LAYER_HPP
#define DXFIO_TABLES_SHAPES_LAYER_HPP

#include "fields/handles.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "record.hpp"

#include "dxfio/tables/layer.hpp"

#include <string_view>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Tables::Layer> = std::tuple{
      Tables::Opening,
      Fields::Subclass{.Marker = "AcDbLayerTableRecord"},
      Fields::Part{
          .Member = &Tables::Layer::Record,
          .Fields = Shapes<Tables::Record>,
      },
      Fields::Field{.Code = 62, .Member = &Tables::Layer::Color},
      Fields::Field{
          .Code   = 420,
          .Member = &Tables::Layer::Rgb,
          .Since  = Version::R2004,
      },
      Fields::Field{
          .Code   = 430,
          .Member = &Tables::Layer::Book,
          .Since  = Version::R2004,
      },
      Fields::Field{.Code = 6, .Member = &Tables::Layer::Linetype},
      Fields::Field{
          .Code     = 290,
          .Member   = &Tables::Layer::Plot,
          .Fallback = true,
          .Since    = Version::R2000,
      },
      Fields::Field{
          .Code   = 370,
          .Member = &Tables::Layer::Lineweight,
          .Since  = Version::R2000,
      },
      Fields::Field{
          .Code   = 390,
          .Member = &Tables::Layer::Style,
          .Since  = Version::R2000,
      },
      Fields::Field{
          .Code     = 347,
          .Member   = &Tables::Layer::Material,
          .Fallback = Handle{},
          .Since    = Version::R2007,
      },
      Fields::Field{
          .Code   = 348,
          .Member = &Tables::Layer::Spare,
          .Since  = Version::R2013,
      },
  };

  template <>
  inline constexpr std::string_view Type<Tables::Layer> = "LAYER";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_LAYER_HPP */
