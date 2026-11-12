#ifndef DXFIO_ENTITIES_SHAPES_MODELER_HPP
#define DXFIO_ENTITIES_SHAPES_MODELER_HPP

#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/owned.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "record.hpp"
#include "shapes.hpp"
#include "trail.hpp"

#include "dxfio/entities/modeler.hpp"
#include "dxfio/version.hpp"

#include <array>
#include <expected>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Entities
  {
    inline constexpr std::array<int, 2> Sentences = {1, 3};

    inline void Lined(Geometry &one, Fields::Walk &in)
    {
      for (const Tags::Tag *held : in.Gather(Sentences))
      {
        one.Lines.push_back(Fields::Owned(*held, in.With().Text));
      }
    }

    inline auto Lining(const Geometry &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      return Fields::Put(to, one.Lines);
    }
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Geometry> = std::tuple{
      Fields::Subclass{.Marker = "AcDbModelerGeometry"},
      Fields::Field{.Code = 70, .Member = &Entities::Geometry::Version},
      Fields::Custom<Entities::Geometry>{
          .Read  = Entities::Lined,
          .Write = Entities::Lining,
      },
      Fields::Field{.Code = 290, .Member = &Entities::Geometry::Stored},
      Fields::Field{.Code = 2, .Member = &Entities::Geometry::Guid},
  };

  namespace Entities
  {
    template <class KindT>
    inline constexpr auto Modeled = std::tuple{
        Part<KindT>,
        Fields::Picture{.Member = &KindT::Picture},
        Fields::Part{
            .Member = &KindT::Geometry,
            .Fields = Shapes<Geometry>,
        },
    };
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Region> = Entities::Modeled<Entities::Region>;

  template <>
  inline constexpr std::string_view Type<Entities::Region> = "REGION";

  template <>
  inline constexpr auto Shapes<Entities::Body> = Entities::Modeled<Entities::Body>;

  template <>
  inline constexpr std::string_view Type<Entities::Body> = "BODY";

  template <>
  inline constexpr auto Shapes<Entities::Solid3d> = std::tuple_cat(
      Entities::Modeled<Entities::Solid3d>, std::tuple{Fields::Since{
                                                .Release = Version::R2007,
                                                .Fields =
                                                    std::tuple{
                                                        Fields::Subclass{.Marker = "AcDb3dSolid"},
                                                        Fields::Field{
                                                            .Code   = 350,
                                                            .Member = &Entities::Solid3d::History,
                                                        },
                                                    },
                                            }});

  template <>
  inline constexpr std::string_view Type<Entities::Solid3d> = "3DSOLID";

  template <Entities::Sheet KindV>
  inline constexpr auto Shapes<Entities::SurfaceOf<KindV>> = std::tuple_cat(
      Entities::Modeled<Entities::SurfaceOf<KindV>>,
      std::tuple{
          Fields::Subclass{.Marker = "AcDbSurface"},
          Fields::Field{
              .Code   = 71,
              .Member = &Entities::SurfaceOf<KindV>::Across,
          },
          Fields::Field{.Code = 72, .Member = &Entities::SurfaceOf<KindV>::Along},
          Entities::Trailed<Entities::SurfaceOf<KindV>, &Entities::SurfaceOf<KindV>::Tail>,
      });

  template <>
  inline constexpr std::string_view Type<Entities::Surface> = "SURFACE";

  template <>
  inline constexpr std::string_view Type<Entities::PlaneSurface> = "PLANESURFACE";

  template <>
  inline constexpr std::string_view Type<Entities::NurbSurface> = "NURBSURFACE";

  template <>
  inline constexpr std::string_view Type<Entities::ExtrudedSurface> = "EXTRUDEDSURFACE";

  template <>
  inline constexpr std::string_view Type<Entities::LoftedSurface> = "LOFTEDSURFACE";

  template <>
  inline constexpr std::string_view Type<Entities::RevolvedSurface> = "REVOLVEDSURFACE";

  template <>
  inline constexpr std::string_view Type<Entities::SweptSurface> = "SWEPTSURFACE";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_MODELER_HPP */
