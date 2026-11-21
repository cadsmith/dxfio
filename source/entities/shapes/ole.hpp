#ifndef DXFIO_ENTITIES_SHAPES_OLE_HPP
#define DXFIO_ENTITIES_SHAPES_OLE_HPP

#include "fields/braced.hpp"
#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/owned.hpp"
#include "fields/part.hpp"
#include "fields/points.hpp"
#include "fields/sequence.hpp"
#include "fields/text.hpp"
#include "record.hpp"
#include "shapes.hpp"
#include "tags/tag.hpp"

#include "dxfio/entities/ole2frame.hpp"
#include "dxfio/tag.hpp"

#include <cstdint>
#include <expected>
#include <string_view>
#include <tuple>
#include <variant>

namespace DxfIO
{
  namespace Entities
  {
    inline constexpr int Sized = 90;

    inline constexpr int Binary = 310;

    inline void Loaded(Ole2FrameData &one, Fields::Walk &in)
    {
      static_cast<void>(Fields::Value<std::int64_t>::Read(in, Sized));
      while (const Tags::Tag *held = in.Take(Binary))
      {
        const Tag part = Fields::Owned(*held, in.With().Text);
        if (const auto *bytes = std::get_if<Chunk>(&part.Value))
        {
          one.Data += bytes->Bytes;
        }
        else
        {
          in.Refuse();
        }
      }
    }

    inline auto Loading(const Ole2FrameData &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      const auto size = static_cast<std::int64_t>(one.Data.size());
      return Fields::Value<std::int64_t>::Write(to, Sized, size)
          .and_then(
              [&] -> std::expected<void, Failure>
              {
                if (one.Data.empty())
                {
                  return {};
                }
                return to.To->Put(Tags::Tag{
                    .Code  = Binary,
                    .Value = Tags::Chunk{std::string_view{one.Data}},
                });
              });
    }
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Ole2FrameData> = std::tuple{
      Fields::Sequence{.Marker = "AcDbOle2Frame"},
      Fields::Field{.Code = 70, .Member = &Entities::Ole2FrameData::Version},
      Fields::Field{.Code = 3, .Member = &Entities::Ole2FrameData::Description},
      Fields::Field{.Code = 10, .Member = &Entities::Ole2FrameData::Upper},
      Fields::Field{.Code = 11, .Member = &Entities::Ole2FrameData::Lower},
      Fields::Field{.Code = 71, .Member = &Entities::Ole2FrameData::Kind},
      Fields::Field{.Code = 72, .Member = &Entities::Ole2FrameData::Space},
      Fields::Custom<Entities::Ole2FrameData>{
          .Read  = Entities::Loaded,
          .Write = Entities::Loading,
      },
      Fields::Literal{.Code = 1, .Text = "OLE"},
  };

  template <>
  inline constexpr auto Shapes<Entities::Ole2Frame> = std::tuple{
      Entities::Part<Entities::Ole2Frame>,
      Fields::Part{
          .Member = &Entities::Ole2Frame::Data,
          .Fields = Shapes<Entities::Ole2FrameData>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::Ole2Frame> = "OLE2FRAME";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_OLE_HPP */
