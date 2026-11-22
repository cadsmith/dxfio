#ifndef DXFIO_ENTITIES_SHAPES_MLEADER_HPP
#define DXFIO_ENTITIES_SHAPES_MLEADER_HPP

#include "fields/braced.hpp"
#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/many.hpp"
#include "fields/matrices.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/part.hpp"
#include "fields/picture.hpp"
#include "fields/points.hpp"
#include "fields/run.hpp"
#include "fields/sequence.hpp"
#include "fields/since.hpp"
#include "fields/text.hpp"
#include "fields/when.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/multileader.hpp"
#include "dxfio/version.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  template <>
  inline constexpr auto Shapes<Entities::MultiLeaders::LineBreak> = std::tuple{
      Fields::Field{.Code = 90, .Member = &Entities::MultiLeaders::LineBreak::Index},
      Fields::Field{.Code = 11, .Member = &Entities::MultiLeaders::LineBreak::Start},
      Fields::Field{.Code = 12, .Member = &Entities::MultiLeaders::LineBreak::End},
  };

  template <>
  inline constexpr auto Shapes<Entities::MultiLeaders::LeaderLine> = std::tuple{
      Fields::Many{.Code = 10, .Member = &Entities::MultiLeaders::LeaderLine::Vertices},
      Fields::Run{
          .Opener = 90,
          .Member = &Entities::MultiLeaders::LeaderLine::Breaks,
          .Fields = Shapes<Entities::MultiLeaders::LineBreak>,
      },
      Fields::Field{.Code = 91, .Member = &Entities::MultiLeaders::LeaderLine::Index},
      Fields::Field{.Code = 92, .Member = &Entities::MultiLeaders::LeaderLine::Color},
      Fields::Field{.Code = 170, .Member = &Entities::MultiLeaders::LeaderLine::LeaderType},
      Fields::Field{.Code     = 340,
                    .Member   = &Entities::MultiLeaders::LeaderLine::Linetype,
                    .Fallback = Handle{}},
      Fields::Field{.Code = 171, .Member = &Entities::MultiLeaders::LeaderLine::Lineweight},
      Fields::Field{.Code = 40, .Member = &Entities::MultiLeaders::LeaderLine::ArrowheadSize},
      Fields::Field{.Code     = 341,
                    .Member   = &Entities::MultiLeaders::LeaderLine::Arrowhead,
                    .Fallback = Handle{}},
      Fields::Field{.Code = 94, .Member = &Entities::MultiLeaders::LeaderLine::Overrides},
  };

  template <>
  inline constexpr auto Shapes<Entities::MultiLeaders::DoglegBreak> = std::tuple{
      Fields::Field{.Code = 12, .Member = &Entities::MultiLeaders::DoglegBreak::Start},
      Fields::Field{.Code = 13, .Member = &Entities::MultiLeaders::DoglegBreak::End},
  };

  template <>
  inline constexpr auto Shapes<Entities::MultiLeaders::LeaderRoot> = std::tuple{
      Fields::Field{.Code = 290, .Member = &Entities::MultiLeaders::LeaderRoot::HasLastPoint},
      Fields::Field{.Code = 291, .Member = &Entities::MultiLeaders::LeaderRoot::HasDoglegVector},
      Fields::Field{.Code = 10, .Member = &Entities::MultiLeaders::LeaderRoot::LastPoint},
      Fields::Field{.Code = 11, .Member = &Entities::MultiLeaders::LeaderRoot::DoglegVector},
      Fields::Run{
          .Opener = 12,
          .Member = &Entities::MultiLeaders::LeaderRoot::Breaks,
          .Fields = Shapes<Entities::MultiLeaders::DoglegBreak>,
      },
      Fields::Field{.Code = 90, .Member = &Entities::MultiLeaders::LeaderRoot::Index},
      Fields::Field{.Code = 40, .Member = &Entities::MultiLeaders::LeaderRoot::DoglegLength},
      Fields::Braced{
          .Opener = 304,
          .Name   = "LEADER_LINE{",
          .Closer = 305,
          .Member = &Entities::MultiLeaders::LeaderRoot::Lines,
          .Fields = Shapes<Entities::MultiLeaders::LeaderLine>,
      },
      Fields::Since{
          .Release = Version::R2010,
          .Fields  = std::tuple{Fields::Field{
              .Code   = 271,
              .Member = &Entities::MultiLeaders::LeaderRoot::AttachmentDirection,
          }},
      },
  };

  namespace Entities
  {
    inline auto Worded(const MultiLeaders::ContextData &one) -> bool
    {
      return one.HasMText;
    }

    inline auto Placing(const MultiLeaders::ContextData &one) -> bool
    {
      return one.HasBlock;
    }

    inline constexpr std::tuple Wording{
        Fields::Field{.Code = 304, .Member = &MultiLeaders::ContextData::Text},
        Fields::Field{.Code = 11, .Member = &MultiLeaders::ContextData::TextNormal},
        Fields::Field{.Code = 340, .Member = &MultiLeaders::ContextData::TextStyle},
        Fields::Field{.Code = 12, .Member = &MultiLeaders::ContextData::TextLocation},
        Fields::Field{.Code = 13, .Member = &MultiLeaders::ContextData::TextDirection},
        Fields::Field{.Code = 42, .Member = &MultiLeaders::ContextData::TextRotation},
        Fields::Field{.Code = 43, .Member = &MultiLeaders::ContextData::BoundaryWidth},
        Fields::Field{.Code = 44, .Member = &MultiLeaders::ContextData::BoundaryHeight},
        Fields::Field{.Code = 45, .Member = &MultiLeaders::ContextData::LineSpacingFactor},
        Fields::Field{.Code = 170, .Member = &MultiLeaders::ContextData::LineSpacingStyle},
        Fields::Field{.Code = 90, .Member = &MultiLeaders::ContextData::TextColor},
        Fields::Field{.Code = 171, .Member = &MultiLeaders::ContextData::TextAttachment},
        Fields::Field{.Code = 172, .Member = &MultiLeaders::ContextData::FlowDirection},
        Fields::Field{.Code = 91, .Member = &MultiLeaders::ContextData::BackgroundColor},
        Fields::Field{.Code = 141, .Member = &MultiLeaders::ContextData::BackgroundScale},
        Fields::Field{.Code = 92, .Member = &MultiLeaders::ContextData::BackgroundTransparency},
        Fields::Field{.Code = 291, .Member = &MultiLeaders::ContextData::BackgroundFill},
        Fields::Field{.Code = 292, .Member = &MultiLeaders::ContextData::BackgroundMask},
        Fields::Field{.Code = 173, .Member = &MultiLeaders::ContextData::ColumnType},
        Fields::Field{.Code = 293, .Member = &MultiLeaders::ContextData::AutoHeight},
        Fields::Field{.Code = 142, .Member = &MultiLeaders::ContextData::ColumnWidth},
        Fields::Field{.Code = 143, .Member = &MultiLeaders::ContextData::ColumnGutter},
        Fields::Field{.Code = 294, .Member = &MultiLeaders::ContextData::ColumnFlowReversed},
        Fields::Many{.Code = 144, .Member = &MultiLeaders::ContextData::ColumnHeights},
        Fields::Field{.Code = 295, .Member = &MultiLeaders::ContextData::WordBreak},
    };

    inline constexpr std::tuple Blocking{
        Fields::Field{.Code = 341, .Member = &MultiLeaders::ContextData::BlockContent},
        Fields::Field{.Code = 14, .Member = &MultiLeaders::ContextData::BlockNormal},
        Fields::Field{.Code = 15, .Member = &MultiLeaders::ContextData::BlockPosition},
        Fields::Field{.Code = 16, .Member = &MultiLeaders::ContextData::BlockScale},
        Fields::Field{.Code = 46, .Member = &MultiLeaders::ContextData::BlockRotation},
        Fields::Field{.Code = 93, .Member = &MultiLeaders::ContextData::BlockColor},
        Fields::Field{.Code = 47, .Member = &MultiLeaders::ContextData::BlockTransform},
    };
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::MultiLeaders::ContextData> = std::tuple{
      Fields::Field{.Code = 40, .Member = &Entities::MultiLeaders::ContextData::ContentScale},
      Fields::Field{.Code = 10, .Member = &Entities::MultiLeaders::ContextData::ContentBasePoint},
      Fields::Field{.Code = 41, .Member = &Entities::MultiLeaders::ContextData::TextHeight},
      Fields::Field{.Code = 140, .Member = &Entities::MultiLeaders::ContextData::ArrowheadSize},
      Fields::Field{.Code = 145, .Member = &Entities::MultiLeaders::ContextData::LandingGap},
      Fields::Field{.Code   = 174,
                    .Member = &Entities::MultiLeaders::ContextData::TextLeftAttachment},
      Fields::Field{.Code   = 175,
                    .Member = &Entities::MultiLeaders::ContextData::TextRightAttachment},
      Fields::Field{.Code = 176, .Member = &Entities::MultiLeaders::ContextData::TextAlignment},
      Fields::Field{.Code = 177, .Member = &Entities::MultiLeaders::ContextData::AttachmentType},
      Fields::Field{.Code = 290, .Member = &Entities::MultiLeaders::ContextData::HasMText},
      Fields::When{.Test = Entities::Worded, .Fields = Entities::Wording},
      Fields::Field{.Code = 296, .Member = &Entities::MultiLeaders::ContextData::HasBlock},
      Fields::When{.Test = Entities::Placing, .Fields = Entities::Blocking},
      Fields::Field{.Code = 110, .Member = &Entities::MultiLeaders::ContextData::PlaneOrigin},
      Fields::Field{.Code = 111, .Member = &Entities::MultiLeaders::ContextData::PlaneXAxis},
      Fields::Field{.Code = 112, .Member = &Entities::MultiLeaders::ContextData::PlaneYAxis},
      Fields::Field{.Code   = 297,
                    .Member = &Entities::MultiLeaders::ContextData::PlaneNormalReversed},
      Fields::Braced{
          .Opener = 302,
          .Name   = "LEADER{",
          .Closer = 303,
          .Member = &Entities::MultiLeaders::ContextData::Leaders,
          .Fields = Shapes<Entities::MultiLeaders::LeaderRoot>,
      },
      Fields::Since{
          .Release = Version::R2010,
          .Fields =
              std::tuple{
                  Fields::Field{.Code = 272,
                                .Member =
                                    &Entities::MultiLeaders::ContextData::TextBottomAttachment},
                  Fields::Field{.Code   = 273,
                                .Member = &Entities::MultiLeaders::ContextData::TextTopAttachment},
              },
      },
  };

  template <>
  inline constexpr auto Shapes<Entities::MultiLeaders::Arrowhead> = std::tuple{
      Fields::Field{.Code = 94, .Member = &Entities::MultiLeaders::Arrowhead::Index},
      Fields::Field{.Code = 345, .Member = &Entities::MultiLeaders::Arrowhead::Block},
  };

  template <>
  inline constexpr auto Shapes<Entities::MultiLeaders::BlockAttribute> = std::tuple{
      Fields::Field{.Code = 330, .Member = &Entities::MultiLeaders::BlockAttribute::Definition},
      Fields::Field{.Code = 177, .Member = &Entities::MultiLeaders::BlockAttribute::Index},
      Fields::Field{.Code = 44, .Member = &Entities::MultiLeaders::BlockAttribute::Width},
      Fields::Field{.Code = 302, .Member = &Entities::MultiLeaders::BlockAttribute::Text},
  };

  template <>
  inline constexpr auto Shapes<Entities::MultiLeaders::MLeader> = std::tuple{
      Fields::Sequence{.Marker = "AcDbMLeader"},
      Fields::Since{
          .Release = Version::R2010,
          .Fields  = std::tuple{Fields::Field{
              .Code   = 270,
              .Member = &Entities::MultiLeaders::MLeader::Version,
          }},
      },
      Fields::Literal{.Code = 300, .Text = "CONTEXT_DATA{"},
      Fields::Part{
          .Member = &Entities::MultiLeaders::MLeader::Context,
          .Fields = Shapes<Entities::MultiLeaders::ContextData>,
      },
      Fields::Literal{.Code = 301, .Text = Fields::Closed},
      Fields::Field{.Code = 340, .Member = &Entities::MultiLeaders::MLeader::Style},
      Fields::Field{.Code = 90, .Member = &Entities::MultiLeaders::MLeader::Overrides},
      Fields::Field{.Code = 170, .Member = &Entities::MultiLeaders::MLeader::LeaderType},
      Fields::Field{.Code = 91, .Member = &Entities::MultiLeaders::MLeader::LineColor},
      Fields::Field{.Code = 341, .Member = &Entities::MultiLeaders::MLeader::Linetype},
      Fields::Field{.Code = 171, .Member = &Entities::MultiLeaders::MLeader::Lineweight},
      Fields::Field{.Code = 290, .Member = &Entities::MultiLeaders::MLeader::EnableLanding},
      Fields::Field{.Code = 291, .Member = &Entities::MultiLeaders::MLeader::EnableDogleg},
      Fields::Field{.Code = 41, .Member = &Entities::MultiLeaders::MLeader::DoglegLength},
      Fields::Field{
          .Code = 342, .Member = &Entities::MultiLeaders::MLeader::Arrowhead, .Fallback = Handle{}},
      Fields::Field{.Code = 42, .Member = &Entities::MultiLeaders::MLeader::ArrowheadSize},
      Fields::Field{.Code = 172, .Member = &Entities::MultiLeaders::MLeader::ContentType},
      Fields::Field{.Code = 343, .Member = &Entities::MultiLeaders::MLeader::TextStyle},
      Fields::Field{.Code = 173, .Member = &Entities::MultiLeaders::MLeader::TextLeftAttachment},
      Fields::Field{.Code = 95, .Member = &Entities::MultiLeaders::MLeader::TextRightAttachment},
      Fields::Field{.Code = 174, .Member = &Entities::MultiLeaders::MLeader::TextAngleType},
      Fields::Field{.Code = 175, .Member = &Entities::MultiLeaders::MLeader::TextAlignment},
      Fields::Field{.Code = 92, .Member = &Entities::MultiLeaders::MLeader::TextColor},
      Fields::Field{.Code = 292, .Member = &Entities::MultiLeaders::MLeader::TextFrame},
      Fields::Field{.Code     = 344,
                    .Member   = &Entities::MultiLeaders::MLeader::BlockContent,
                    .Fallback = Handle{}},
      Fields::Field{.Code = 93, .Member = &Entities::MultiLeaders::MLeader::BlockColor},
      Fields::Field{.Code = 10, .Member = &Entities::MultiLeaders::MLeader::BlockScale},
      Fields::Field{.Code = 43, .Member = &Entities::MultiLeaders::MLeader::BlockRotation},
      Fields::Field{.Code = 176, .Member = &Entities::MultiLeaders::MLeader::BlockConnection},
      Fields::Field{.Code = 293, .Member = &Entities::MultiLeaders::MLeader::Annotative},
      Fields::Run{
          .Opener = 94,
          .Member = &Entities::MultiLeaders::MLeader::Arrowheads,
          .Fields = Shapes<Entities::MultiLeaders::Arrowhead>,
      },
      Fields::Run{
          .Opener = 330,
          .Member = &Entities::MultiLeaders::MLeader::BlockAttributes,
          .Fields = Shapes<Entities::MultiLeaders::BlockAttribute>,
      },
      Fields::Field{.Code = 294, .Member = &Entities::MultiLeaders::MLeader::TextDirectionNegative},
      Fields::Field{.Code = 178, .Member = &Entities::MultiLeaders::MLeader::IpeAlignment},
      Fields::Field{.Code = 179, .Member = &Entities::MultiLeaders::MLeader::TextAttachmentPoint},
      Fields::Field{.Code = 45, .Member = &Entities::MultiLeaders::MLeader::OverallScale},
      Fields::Since{
          .Release = Version::R2010,
          .Fields =
              std::tuple{
                  Fields::Field{
                      .Code   = 271,
                      .Member = &Entities::MultiLeaders::MLeader::TextAttachmentDirection,
                  },
                  Fields::Field{.Code   = 272,
                                .Member = &Entities::MultiLeaders::MLeader::TextBottomAttachment},
                  Fields::Field{.Code   = 273,
                                .Member = &Entities::MultiLeaders::MLeader::TextTopAttachment},
              },
      },
      Fields::Since{
          .Release = Version::R2013,
          .Fields  = std::tuple{Fields::Field{
              .Code   = 295,
              .Member = &Entities::MultiLeaders::MLeader::ExtendToText,
          }},
      },
  };

  template <>
  inline constexpr auto Shapes<Entities::MultiLeader> = std::tuple{
      Entities::Part<Entities::MultiLeader>,
      Fields::Picture{.Member = &Entities::MultiLeader::Graphics},
      Fields::Part{
          .Member = &Entities::MultiLeader::MLeader,
          .Fields = Shapes<Entities::MultiLeaders::MLeader>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::MultiLeader> = "MULTILEADER";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_MLEADER_HPP */
