#ifndef DXFIO_ENTITIES_SHAPES_SLICE_HPP
#define DXFIO_ENTITIES_SHAPES_SLICE_HPP

#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/tally.hpp"
#include "fields/text.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/section.hpp"
#include "dxfio/version.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::Cut> = std::tuple{
      Fields::Subclass{.Marker = "AcDbSection"},
      Fields::Field{.Code = 90, .Member = &Entities::Cut::State},
      Fields::Field{.Code = 91, .Member = &Entities::Cut::Flags},
      Fields::Field{.Code = 1, .Member = &Entities::Cut::Name},
      Fields::Field{.Code = 10, .Member = &Entities::Cut::Vertical},
      Fields::Field{.Code = 40, .Member = &Entities::Cut::Top},
      Fields::Field{.Code = 41, .Member = &Entities::Cut::Bottom},
      Fields::Field{.Code = 70, .Member = &Entities::Cut::Transparency},
      Fields::Since{
          .Release = Version::R2004,
          .Fields =
              std::tuple{
                  Fields::Field{.Code = 62, .Member = &Entities::Cut::Color},
                  Fields::Field{.Code = 420, .Member = &Entities::Cut::Rgb},
              },
      },
      Fields::Tally{.Code = 92, .Member = &Entities::Cut::Vertices},
      Fields::Many{.Code = 11, .Member = &Entities::Cut::Vertices},
      Fields::Tally{.Code = 93, .Member = &Entities::Cut::Back},
      Fields::Many{.Code = 12, .Member = &Entities::Cut::Back},
      Fields::Field{.Code = 360, .Member = &Entities::Cut::Settings},
  };

  template <>
  inline constexpr auto Shapes<Entities::SectionObject> = std::tuple{
      Entities::Part<Entities::SectionObject>,
      Fields::Picture{.Member = &Entities::SectionObject::Picture},
      Fields::Part{
          .Member = &Entities::SectionObject::Cut,
          .Fields = Shapes<Entities::Cut>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::SectionObject> = "SECTIONOBJECT";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_SLICE_HPP */
