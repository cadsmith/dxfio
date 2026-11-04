#ifndef DXFIO_TABLES_SHAPES_BLOCK_HPP
#define DXFIO_TABLES_SHAPES_BLOCK_HPP

#include "fields/braced.hpp"
#include "fields/context.hpp"
#include "fields/custom.hpp"
#include "fields/handles.hpp"
#include "fields/many.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "fields/text.hpp"
#include "fields/walk.hpp"
#include "record.hpp"
#include "tags/codes.hpp"
#include "tags/tag.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/tables/block.hpp"
#include "dxfio/version.hpp"

#include <cstddef>
#include <expected>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

namespace DxfIO
{
namespace Tables
{
inline constexpr std::string_view Blkrefs = "{BLKREFS";

inline constexpr Fields::Many Inserted{.Code = 331, .Member = &BlockRecord::Inserts};

inline auto Ends(Fields::Walk &in, const Fields::Walk::Fence &outer) -> bool
{
  const std::size_t at = in.Fenced().End;
  if (at >= outer.End)
  {
    return false;
  }
  in.Restore(Fields::Walk::Fence{.Begin = at, .End = at + 1, .Cursor = at});
  return Fields::Value<std::string>::Read(in, Tags::Codes::Control) == Fields::Closed;
}

inline auto Listed(BlockRecord &one, Fields::Walk &in) -> bool
{
  const Fields::Walk::Fence outer = in.Fenced();
  in.Rewind();
  in.Narrow(Tags::Codes::Control, outer);
  const Fields::Walk::Fence brace = in.Fenced();
  const Tags::Tag *opener = in.Take(Tags::Codes::Control);
  if (opener == nullptr || !Tags::Holds(*opener, Tags::Codes::Control, Blkrefs))
  {
    return false;
  }
  in.Restore(brace);
  Fields::Take(Inserted, one, in);
  const bool whole = one.Inserts.size() + 1 == brace.End - brace.Begin && Ends(in, outer);
  in.Restore(outer);
  return whole;
}

inline void Referenced(BlockRecord &one, Fields::Walk &in)
{
  Fields::Walk trial = in;
  BlockRecord read{};
  if (Listed(read, trial))
  {
    one.Inserts = std::move(read.Inserts);
    in = std::move(trial);
  }
}

inline auto Referencing(const BlockRecord &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  if (one.Inserts.empty())
  {
    return {};
  }
  return Fields::Word(to, Tags::Codes::Control, Blkrefs)
      .and_then([&to, &one] -> std::expected<void, Failure> { return Fields::Put(Inserted, one, to); })
      .and_then([&to] -> std::expected<void, Failure>
                { return Fields::Word(to, Tags::Codes::Control, Fields::Closed); });
}
} // namespace Tables

template <>
inline constexpr auto Shapes<Tables::BlockRecord> = std::tuple{
    Tables::Opening,
    Fields::Subclass{.Marker = "AcDbBlockTableRecord"},
    Fields::Part{
        .Member = &Tables::BlockRecord::Record,
        .Fields = std::tuple{Fields::Field{
            .Code = 2,
            .Member = &Tables::Record::Name,
        }},
    },
    Fields::Since{
        .Release = Version::R2000,
        .Fields =
            std::tuple{
                Fields::Field{.Code = 340, .Member = &Tables::BlockRecord::Layout},
                Fields::Custom<Tables::BlockRecord>{
                    .Read = Tables::Referenced,
                    .Write = Tables::Referencing,
                },
            },
    },
    Fields::Field{
        .Code = 70,
        .Member = &Tables::BlockRecord::Units,
        .Since = Version::R2007,
    },
    Fields::Field{
        .Code = 280,
        .Member = &Tables::BlockRecord::Explodable,
        .Since = Version::R2007,
    },
    Fields::Field{
        .Code = 281,
        .Member = &Tables::BlockRecord::Uniform,
        .Since = Version::R2007,
    },
};

template <>
inline constexpr std::string_view Type<Tables::BlockRecord> = "BLOCK_RECORD";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_BLOCK_HPP */
