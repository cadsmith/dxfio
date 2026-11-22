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
  inline constexpr auto Shapes<Entities::SectionObjectData> = std::tuple{
      Fields::Subclass{.Marker = "AcDbSection"},
      Fields::Field{.Code = 90, .Member = &Entities::SectionObjectData::State},
      Fields::Field{.Code = 91, .Member = &Entities::SectionObjectData::Flags},
      Fields::Field{.Code = 1, .Member = &Entities::SectionObjectData::Name},
      Fields::Field{.Code = 10, .Member = &Entities::SectionObjectData::Vertical},
      Fields::Field{.Code = 40, .Member = &Entities::SectionObjectData::Top},
      Fields::Field{.Code = 41, .Member = &Entities::SectionObjectData::Bottom},
      Fields::Field{.Code = 70, .Member = &Entities::SectionObjectData::Transparency},
      Fields::Since{
          .Release = Version::R2004,
          .Fields =
              std::tuple{
                  Fields::Field{.Code = 62, .Member = &Entities::SectionObjectData::Color},
                  Fields::Field{.Code = 420, .Member = &Entities::SectionObjectData::Rgb},
              },
      },
      Fields::Tally{.Code = 92, .Member = &Entities::SectionObjectData::Vertices},
      Fields::Many{.Code = 11, .Member = &Entities::SectionObjectData::Vertices},
      Fields::Tally{.Code = 93, .Member = &Entities::SectionObjectData::Back},
      Fields::Many{.Code = 12, .Member = &Entities::SectionObjectData::Back},
      Fields::Field{.Code = 360, .Member = &Entities::SectionObjectData::Settings},
  };

  template <>
  inline constexpr auto Shapes<Entities::SectionObject> = std::tuple{
      Entities::Part<Entities::SectionObject>,
      Fields::Picture{.Member = &Entities::SectionObject::Picture},
      Fields::Part{
          .Member = &Entities::SectionObject::Data,
          .Fields = Shapes<Entities::SectionObjectData>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::SectionObject> = "SECTIONOBJECT";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_SLICE_HPP */
