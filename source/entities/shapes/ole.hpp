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
#include "dxfio/group.hpp"

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

inline void Loaded(Frame &one, Fields::Walk &in)
{
  static_cast<void>(Fields::Value<std::int64_t>::Read(in, Sized));
  while (const Tags::Tag *held = in.Take(Binary))
  {
    const Group part = Fields::Owned(*held, in.With().Text);
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

inline auto Loading(const Frame &one, Fields::Writing &to) -> std::expected<void, Failure>
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
                .Code = Binary,
                .Value = Tags::Chunk{std::string_view{one.Data}},
            });
          });
}
} // namespace Entities

template <>
inline constexpr auto Shapes<Entities::Frame> = std::tuple{
    Fields::Sequence{.Marker = "AcDbOle2Frame"},
    Fields::Field{.Code = 70, .Member = &Entities::Frame::Version},
    Fields::Field{.Code = 3, .Member = &Entities::Frame::Description},
    Fields::Field{.Code = 10, .Member = &Entities::Frame::Upper},
    Fields::Field{.Code = 11, .Member = &Entities::Frame::Lower},
    Fields::Field{.Code = 71, .Member = &Entities::Frame::Kind},
    Fields::Field{.Code = 72, .Member = &Entities::Frame::Space},
    Fields::Custom<Entities::Frame>{
        .Read = Entities::Loaded,
        .Write = Entities::Loading,
    },
    Fields::Literal{.Code = 1, .Text = "OLE"},
};

template <>
inline constexpr auto Shapes<Entities::Ole2Frame> = std::tuple{
    Entities::Part<Entities::Ole2Frame>,
    Fields::Part{
        .Member = &Entities::Ole2Frame::Frame,
        .Fields = Shapes<Entities::Frame>,
    },
};

template <>
inline constexpr std::string_view Type<Entities::Ole2Frame> = "OLE2FRAME";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_OLE_HPP */
