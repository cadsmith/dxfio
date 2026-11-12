#ifndef DXFIO_BLOCKS_SHAPES_BEGIN_HPP
#define DXFIO_BLOCKS_SHAPES_BEGIN_HPP

#include "../../entities/shapes/record.hpp"
#include "fields/context.hpp"
#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/points.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "fields/walk.hpp"
#include "shapes.hpp"

#include "dxfio/blocks/begin.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/version.hpp"

#include <expected>
#include <string>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Blocks
  {
    inline void Again(Begin & /*unused*/, Fields::Walk &in)
    {
      static_cast<void>(in.Take(3));
    }

    inline auto Named(const Begin &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      return Fields::Value<std::string>::Write(to, 3, one.Name);
    }
  } // namespace Blocks

  template <>
  inline constexpr auto Shapes<Blocks::Begin> = std::tuple{
      Entities::Part<Blocks::Begin>,
      Fields::Subclass{.Marker = "AcDbBlockBegin"},
      Fields::Field{.Code = 2, .Member = &Blocks::Begin::Name},
      Fields::Field{.Code = 70, .Member = &Blocks::Begin::Flags},
      Fields::Field{.Code = 10, .Member = &Blocks::Begin::Origin},
      Fields::Custom<Blocks::Begin>{
          .Read  = Blocks::Again,
          .Write = Blocks::Named,
      },
      Fields::Field{.Code = 1, .Member = &Blocks::Begin::Path},
      Fields::Since{
          .Release = Version::R2000,
          .Fields  = std::tuple{Fields::Field{
              .Code     = 4,
              .Member   = &Blocks::Begin::Description,
              .Fallback = std::string_view{},
          }},
      },
  };

  template <>
  inline constexpr std::string_view Type<Blocks::Begin> = "BLOCK";
} // namespace DxfIO

#endif /* DXFIO_BLOCKS_SHAPES_BEGIN_HPP */
