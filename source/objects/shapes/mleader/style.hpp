#ifndef DXFIO_OBJECTS_SHAPES_MLEADER_STYLE_HPP
#define DXFIO_OBJECTS_SHAPES_MLEADER_STYLE_HPP

#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/handle.hpp"
#include "dxfio/objects/mleader/style.hpp"
#include "dxfio/vector.hpp"
#include "dxfio/version.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Objects::MLeaderStyles
  {
    inline constexpr auto Lines = std::tuple{
        Fields::Field{.Code = 172, .Member = &LeaderFormat::DrawOrder},
        Fields::Field{.Code = 90, .Member = &LeaderFormat::MaxPoints},
        Fields::Field{.Code = 40, .Member = &LeaderFormat::FirstSegmentAngle},
        Fields::Field{.Code = 41, .Member = &LeaderFormat::SecondSegmentAngle},
        Fields::Field{.Code = 173, .Member = &LeaderFormat::LeaderType},
        Fields::Field{.Code = 91, .Member = &LeaderFormat::Color},
        Fields::Field{.Code = 340, .Member = &LeaderFormat::Linetype},
        Fields::Field{.Code = 92, .Member = &LeaderFormat::Lineweight},
        Fields::Field{.Code = 290, .Member = &LeaderFormat::EnableLanding},
        Fields::Field{.Code = 42, .Member = &LeaderFormat::LandingGap},
        Fields::Field{.Code = 291, .Member = &LeaderFormat::EnableDogleg},
        Fields::Field{.Code = 43, .Member = &LeaderFormat::DoglegLength},
    };

    inline constexpr auto Heads = std::tuple{
        Fields::Field{.Code = 341, .Member = &LeaderFormat::Arrowhead, .Fallback = Handle{}},
        Fields::Field{.Code = 44, .Member = &LeaderFormat::ArrowheadSize},
    };

    inline constexpr auto Words = std::tuple{
        Fields::Field{.Code = 300, .Member = &TextFormat::DefaultText},
        Fields::Field{.Code = 342, .Member = &TextFormat::Style},
        Fields::Field{.Code = 174, .Member = &TextFormat::LeftAttachment},
        Fields::Field{.Code = 178, .Member = &TextFormat::RightAttachment},
        Fields::Field{.Code = 175, .Member = &TextFormat::AngleType},
        Fields::Field{.Code = 176, .Member = &TextFormat::Alignment},
        Fields::Field{.Code = 93, .Member = &TextFormat::Color},
        Fields::Field{.Code = 45, .Member = &TextFormat::Height},
        Fields::Field{.Code = 292, .Member = &TextFormat::Frame},
        Fields::Field{.Code = 297, .Member = &TextFormat::AlwaysLeft},
        Fields::Field{.Code = 46, .Member = &TextFormat::AlignSpace},
    };

    inline constexpr auto Attachments = std::tuple{
        Fields::Field{.Code = 271, .Member = &TextFormat::AttachmentDirection},
        Fields::Field{.Code = 272, .Member = &TextFormat::BottomAttachment},
        Fields::Field{.Code = 273, .Member = &TextFormat::TopAttachment},
    };

    inline constexpr auto Shown = std::tuple{
        Fields::Field{.Code = 343, .Member = &BlockFormat::Content, .Fallback = Handle{}},
        Fields::Field{.Code = 94, .Member = &BlockFormat::Color},
        Fields::Part{
            .Member = &BlockFormat::Scale,
            .Fields =
                std::tuple{
                    Fields::Field<Vector, double>{.Code = 47, .Member = &Vector::X},
                    Fields::Field<Vector, double>{.Code = 49, .Member = &Vector::Y},
                    Fields::Field<Vector, double>{
                        .Code   = 140,
                        .Member = &Vector::Z,
                    },
                },
        },
        Fields::Field{.Code = 293, .Member = &BlockFormat::EnableScale},
        Fields::Field{.Code = 141, .Member = &BlockFormat::Rotation},
        Fields::Field{.Code = 294, .Member = &BlockFormat::EnableRotation},
        Fields::Field{.Code = 177, .Member = &BlockFormat::Connection},
    };
  } // namespace Objects::MLeaderStyles

  template <>
  inline constexpr auto Shapes<Objects::MLeaderStyle> = std::tuple{
      Fields::Subclass{.Marker = "AcDbMLeaderStyle"},
      Fields::Since{
          .Release = Version::R2010,
          .Fields  = std::tuple{Fields::Field{
              .Code   = 179,
              .Member = &Objects::MLeaderStyle::Revision,
          }},
      },
      Fields::Field{.Code = 170, .Member = &Objects::MLeaderStyle::ContentType},
      Fields::Field{.Code = 171, .Member = &Objects::MLeaderStyle::DrawOrder},
      Fields::Part{
          .Member = &Objects::MLeaderStyle::Leader,
          .Fields = Objects::MLeaderStyles::Lines,
      },
      Fields::Field{.Code = 3, .Member = &Objects::MLeaderStyle::Description},
      Fields::Part{
          .Member = &Objects::MLeaderStyle::Leader,
          .Fields = Objects::MLeaderStyles::Heads,
      },
      Fields::Part{
          .Member = &Objects::MLeaderStyle::Text,
          .Fields = Objects::MLeaderStyles::Words,
      },
      Fields::Part{
          .Member = &Objects::MLeaderStyle::Block,
          .Fields = Objects::MLeaderStyles::Shown,
      },
      Fields::Field{.Code = 142, .Member = &Objects::MLeaderStyle::Scale},
      Fields::Field{.Code = 295, .Member = &Objects::MLeaderStyle::OverwriteProperty},
      Fields::Field{.Code = 296, .Member = &Objects::MLeaderStyle::Annotative},
      Fields::Field{.Code = 143, .Member = &Objects::MLeaderStyle::BreakGapSize},
      Fields::Since{
          .Release = Version::R2010,
          .Fields  = std::tuple{Fields::Part{
              .Member = &Objects::MLeaderStyle::Text,
              .Fields = Objects::MLeaderStyles::Attachments,
          }},
      },
      Fields::Since{
          .Release = Version::R2013,
          .Fields  = std::tuple{Fields::Field{
              .Code   = 298,
              .Member = &Objects::MLeaderStyle::Unknown1,
          }},
      },
  };

  template <>
  inline constexpr std::string_view Type<Objects::MLeaderStyle> = "MLEADERSTYLE";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_MLEADER_STYLE_HPP */
