#ifndef DXFIO_OBJECTS_SHAPES_LOOK_HPP
#define DXFIO_OBJECTS_SHAPES_LOOK_HPP

#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/owned.hpp"
#include "fields/sequence.hpp"
#include "fields/since.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/objects/visualstyle.hpp"
#include "dxfio/version.hpp"

#include <expected>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Objects
  {
    inline void Propertied(VisualStyle &one, Fields::Walk &in)
    {
      one.Properties = in.Rest();
      in.Spend();
    }

    inline auto Properties(const VisualStyle &one, Fields::Writing &to)
        -> std::expected<void, Failure>
    {
      return Fields::Put(to, one.Properties);
    }
  } // namespace Objects

  template <>
  inline constexpr auto Shapes<Objects::VisualStyle> = std::tuple{
      Fields::Sequence{.Marker = "AcDbVisualStyle"},
      Fields::Field{.Code = 2, .Member = &Objects::VisualStyle::Name},
      Fields::Field{.Code = 70, .Member = &Objects::VisualStyle::Kind},
      Fields::Since{
          .Release = Version::R2010,
          .Fields =
              std::tuple{
                  Fields::Field{.Code = 177, .Member = &Objects::VisualStyle::Version},
                  Fields::Field{.Code = 291, .Member = &Objects::VisualStyle::Internal},
              },
      },
      Fields::Custom<Objects::VisualStyle>{
          .Read  = Objects::Propertied,
          .Write = Objects::Properties,
      },
  };

  template <>
  inline constexpr std::string_view Type<Objects::VisualStyle> = "VISUALSTYLE";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_LOOK_HPP */
