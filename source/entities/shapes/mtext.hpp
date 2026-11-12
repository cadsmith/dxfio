#ifndef DXFIO_ENTITIES_SHAPES_MTEXT_HPP
#define DXFIO_ENTITIES_SHAPES_MTEXT_HPP

#include "fields/custom.hpp"
#include "fields/embedded.hpp"
#include "fields/field.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/pieces.hpp"
#include "fields/points.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "masked.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/mtext.hpp"
#include "dxfio/vector.hpp"
#include "dxfio/version.hpp"

#include <expected>
#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Entities
  {
    inline constexpr int Directed = 11;

    inline constexpr int Rotated = 50;

    inline void Turned(Paragraph &one, Fields::Walk &in)
    {
      const bool last = in.Later(Rotated, Directed);
      one.Direction   = Fields::Value<Vector>::Read(in, Directed);
      one.Rotation    = Fields::Value<double>::Read(in, Rotated);
      if (last)
      {
        one.Direction.reset();
      }
      else
      {
        one.Rotation.reset();
      }
    }

    inline auto Turning(const Paragraph &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      using Direction = Fields::Value<std::optional<Vector>>;
      using Rotation  = Fields::Value<std::optional<double>>;
      return Direction::Write(to, Directed, one.Direction)
          .and_then([&] -> std::expected<void, Failure>
                    { return Rotation::Write(to, Rotated, one.Rotation); });
    }

    inline void Barred(Columns & /*unused*/, Fields::Walk &in)
    {
      for (const int code : {49, 75, 76, 78, 79})
      {
        in.Expect(code);
      }
    }

    inline auto Unbarred(const Columns & /*unused*/, Fields::Writing & /*unused*/)
        -> std::expected<void, Failure>
    {
      return {};
    }

    inline constexpr int Painted = 1;

    inline constexpr int Bordered = 16;

    inline constexpr std::tuple Backing{
        Fields::Field{.Code = 63, .Member = &Paragraph::Color},
        Fields::Field{.Code = 421, .Member = &Paragraph::Rgb},
        Fields::Field{.Code = 431, .Member = &Paragraph::Book},
        Fields::Field{.Code = 45, .Member = &Paragraph::Margin},
        Fields::Field{.Code = 441, .Member = &Paragraph::Transparency},
    };

    inline void Backed(Paragraph &one, Fields::Walk &in)
    {
      Fields::Read(one, in, Backing);
    }

    inline auto Backs(const Paragraph &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      const int wanted = to.Release >= Version::R2018 ? Painted + Bordered : Painted;
      return Masked(one.Fill, wanted) != 0 ? Fields::Write(one, to, Backing)
                                           : std::expected<void, Failure>{};
    }

    inline constexpr std::tuple Background{
        Fields::Field{.Code = 90, .Member = &Paragraph::Fill, .Fallback = 0},
        Fields::Custom<Paragraph>{.Read = Backed, .Write = Backs},
    };
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Columns> = std::tuple{
      Fields::Field{.Code = 70, .Member = &Entities::Columns::Attachment},
      Fields::Field{.Code = 10, .Member = &Entities::Columns::Direction},
      Fields::Field{.Code = 11, .Member = &Entities::Columns::Position},
      Fields::Field{.Code = 40, .Member = &Entities::Columns::Width},
      Fields::Field{.Code = 41, .Member = &Entities::Columns::Height},
      Fields::Field{.Code = 42, .Member = &Entities::Columns::Extent},
      Fields::Field{.Code = 43, .Member = &Entities::Columns::Depth},
      Fields::Field{.Code = 71, .Member = &Entities::Columns::Kind},
      Fields::Field{.Code = 72, .Member = &Entities::Columns::Count},
      Fields::Field{.Code = 44, .Member = &Entities::Columns::Breadth},
      Fields::Field{.Code = 45, .Member = &Entities::Columns::Gutter},
      Fields::Field{.Code = 73, .Member = &Entities::Columns::Automatic},
      Fields::Field{.Code = 74, .Member = &Entities::Columns::Reversed},
      Fields::Many{.Code = 46, .Member = &Entities::Columns::Heights},
      Fields::Custom<Entities::Columns>{
          .Read  = Entities::Barred,
          .Write = Entities::Unbarred,
      },
  };

  template <>
  inline constexpr auto Shapes<Entities::Paragraph> = std::tuple{
      Fields::Subclass{.Marker = "AcDbMText"},
      Fields::Field{.Code = 10, .Member = &Entities::Paragraph::Position},
      Fields::Field{.Code = 40, .Member = &Entities::Paragraph::Height},
      Fields::Field{.Code = 41, .Member = &Entities::Paragraph::Width},
      Fields::Field{
          .Code   = 46,
          .Member = &Entities::Paragraph::Defined,
          .Since  = Version::R2007,
      },
      Fields::Field{.Code = 71, .Member = &Entities::Paragraph::Attachment},
      Fields::Field{.Code = 72, .Member = &Entities::Paragraph::Flow},
      Fields::Pieces{.Member = &Entities::Paragraph::Value},
      Fields::Field{
          .Code     = 7,
          .Member   = &Entities::Paragraph::Style,
          .Fallback = std::string_view{"Standard"},
      },
      Fields::Field{
          .Code     = 210,
          .Member   = &Entities::Paragraph::Extrusion,
          .Fallback = Up,
      },
      Fields::Custom<Entities::Paragraph>{
          .Read  = Entities::Turned,
          .Write = Entities::Turning,
      },
      Fields::Field{.Code = 73, .Member = &Entities::Paragraph::Spacing},
      Fields::Field{.Code = 44, .Member = &Entities::Paragraph::Factor},
      Fields::Since{.Release = Version::R2004, .Fields = Entities::Background},
      Fields::Since{
          .Release = Version::R2018,
          .Fields  = std::tuple{Fields::Embedded{
              .Member = &Entities::Paragraph::Columns,
              .Fields = Shapes<Entities::Columns>,
          }},
      },
  };

  template <>
  inline constexpr auto Shapes<Entities::MText> = std::tuple{
      Entities::Part<Entities::MText>,
      Fields::Part{
          .Member = &Entities::MText::Paragraph,
          .Fields = Shapes<Entities::Paragraph>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::MText> = "MTEXT";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_MTEXT_HPP */
