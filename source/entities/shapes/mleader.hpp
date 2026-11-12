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
  inline constexpr auto Shapes<Entities::Gap> = std::tuple{
      Fields::Field{.Code = 90, .Member = &Entities::Gap::Index},
      Fields::Field{.Code = 11, .Member = &Entities::Gap::Start},
      Fields::Field{.Code = 12, .Member = &Entities::Gap::End},
  };

  template <>
  inline constexpr auto Shapes<Entities::Stem> = std::tuple{
      Fields::Many{.Code = 10, .Member = &Entities::Stem::Points},
      Fields::Run{
          .Opener = 90,
          .Member = &Entities::Stem::Gaps,
          .Fields = Shapes<Entities::Gap>,
      },
      Fields::Field{.Code = 91, .Member = &Entities::Stem::Index},
      Fields::Field{.Code = 92, .Member = &Entities::Stem::Color},
      Fields::Field{.Code = 170, .Member = &Entities::Stem::Kind},
      Fields::Field{.Code = 340, .Member = &Entities::Stem::Linetype, .Fallback = Handle{}},
      Fields::Field{.Code = 171, .Member = &Entities::Stem::Lineweight},
      Fields::Field{.Code = 40, .Member = &Entities::Stem::Arrow},
      Fields::Field{.Code = 341, .Member = &Entities::Stem::Head, .Fallback = Handle{}},
      Fields::Field{.Code = 94, .Member = &Entities::Stem::Overrides},
  };

  template <>
  inline constexpr auto Shapes<Entities::Span> = std::tuple{
      Fields::Field{.Code = 12, .Member = &Entities::Span::Start},
      Fields::Field{.Code = 13, .Member = &Entities::Span::End},
  };

  template <>
  inline constexpr auto Shapes<Entities::Branch> = std::tuple{
      Fields::Field{.Code = 290, .Member = &Entities::Branch::Lasting},
      Fields::Field{.Code = 291, .Member = &Entities::Branch::Bent},
      Fields::Field{.Code = 10, .Member = &Entities::Branch::Last},
      Fields::Field{.Code = 11, .Member = &Entities::Branch::Dogleg},
      Fields::Run{
          .Opener = 12,
          .Member = &Entities::Branch::Spans,
          .Fields = Shapes<Entities::Span>,
      },
      Fields::Field{.Code = 90, .Member = &Entities::Branch::Index},
      Fields::Field{.Code = 40, .Member = &Entities::Branch::Length},
      Fields::Braced{
          .Opener = 304,
          .Name   = "LEADER_LINE{",
          .Closer = 305,
          .Member = &Entities::Branch::Stems,
          .Fields = Shapes<Entities::Stem>,
      },
      Fields::Since{
          .Release = Version::R2010,
          .Fields  = std::tuple{Fields::Field{
              .Code   = 271,
              .Member = &Entities::Branch::Attachment,
          }},
      },
  };

  namespace Entities
  {
    inline auto Worded(const Note &one) -> bool
    {
      return one.Texted;
    }

    inline auto Placing(const Note &one) -> bool
    {
      return one.Blocked;
    }

    inline constexpr std::tuple Wording{
        Fields::Field{.Code = 304, .Member = &Note::Text},
        Fields::Field{.Code = 11, .Member = &Note::Normal},
        Fields::Field{.Code = 340, .Member = &Note::Style},
        Fields::Field{.Code = 12, .Member = &Note::Location},
        Fields::Field{.Code = 13, .Member = &Note::Direction},
        Fields::Field{.Code = 42, .Member = &Note::Rotation},
        Fields::Field{.Code = 43, .Member = &Note::Width},
        Fields::Field{.Code = 44, .Member = &Note::Tall},
        Fields::Field{.Code = 45, .Member = &Note::Spacing},
        Fields::Field{.Code = 170, .Member = &Note::Spaced},
        Fields::Field{.Code = 90, .Member = &Note::Color},
        Fields::Field{.Code = 171, .Member = &Note::Attachment},
        Fields::Field{.Code = 172, .Member = &Note::Flow},
        Fields::Field{.Code = 91, .Member = &Note::Background},
        Fields::Field{.Code = 141, .Member = &Note::Margin},
        Fields::Field{.Code = 92, .Member = &Note::Transparency},
        Fields::Field{.Code = 291, .Member = &Note::Backed},
        Fields::Field{.Code = 292, .Member = &Note::Filled},
        Fields::Field{.Code = 173, .Member = &Note::Columns},
        Fields::Field{.Code = 293, .Member = &Note::Grows},
        Fields::Field{.Code = 142, .Member = &Note::Column},
        Fields::Field{.Code = 143, .Member = &Note::Gutter},
        Fields::Field{.Code = 294, .Member = &Note::Reversed},
        Fields::Many{.Code = 144, .Member = &Note::Heights},
        Fields::Field{.Code = 295, .Member = &Note::Breaks},
    };

    inline constexpr std::tuple Blocking{
        Fields::Field{.Code = 341, .Member = &Note::Block},
        Fields::Field{.Code = 14, .Member = &Note::Facing},
        Fields::Field{.Code = 15, .Member = &Note::Placed},
        Fields::Field{.Code = 16, .Member = &Note::Sized},
        Fields::Field{.Code = 46, .Member = &Note::Turned},
        Fields::Field{.Code = 93, .Member = &Note::Tint},
        Fields::Field{.Code = 47, .Member = &Note::Frame},
    };
  } // namespace Entities

  template <>
  inline constexpr auto Shapes<Entities::Note> =
      std::tuple{
          Fields::Field{.Code = 40, .Member = &Entities::Note::Scale},
          Fields::Field{.Code = 10, .Member = &Entities::Note::Base},
          Fields::Field{.Code = 41, .Member = &Entities::Note::Height},
          Fields::Field{.Code = 140, .Member = &Entities::Note::Arrow},
          Fields::Field{.Code = 145, .Member = &Entities::Note::Landing},
          Fields::Field{.Code = 174, .Member = &Entities::Note::Left},
          Fields::Field{.Code = 175, .Member = &Entities::Note::Right},
          Fields::Field{.Code = 176, .Member = &Entities::Note::Align},
          Fields::Field{.Code = 177, .Member = &Entities::Note::Attach},
          Fields::Field{.Code = 290, .Member = &Entities::Note::Texted},
          Fields::When{.Test = Entities::Worded, .Fields = Entities::Wording},
          Fields::Field{.Code = 296, .Member = &Entities::Note::Blocked},
          Fields::When{.Test = Entities::Placing, .Fields = Entities::Blocking},
          Fields::Field{.Code = 110, .Member = &Entities::Note::Origin},
          Fields::Field{.Code = 111, .Member = &Entities::Note::Across},
          Fields::Field{.Code = 112, .Member = &Entities::Note::Along},
          Fields::Field{.Code = 297, .Member = &Entities::Note::Flipped},
          Fields::Braced{
              .Opener = 302,
              .Name   = "LEADER{",
              .Closer = 303,
              .Member = &Entities::Note::Branches,
              .Fields = Shapes<Entities::Branch>,
          },
          Fields::Since{
              .Release = Version::R2010,
              .Fields =
                  std::tuple{
                      Fields::Field{.Code = 272, .Member = &Entities::Note::Bottom},
                      Fields::Field{.Code = 273, .Member = &Entities::Note::Top},
                  },
          },
  };

  template <>
  inline constexpr auto Shapes<Entities::Tip> = std::tuple{
      Fields::Field{.Code = 94, .Member = &Entities::Tip::Index},
      Fields::Field{.Code = 345, .Member = &Entities::Tip::Block},
  };

  template <>
  inline constexpr auto Shapes<Entities::Label> = std::tuple{
      Fields::Field{.Code = 330, .Member = &Entities::Label::Definition},
      Fields::Field{.Code = 177, .Member = &Entities::Label::Index},
      Fields::Field{.Code = 44, .Member = &Entities::Label::Width},
      Fields::Field{.Code = 302, .Member = &Entities::Label::Text},
  };

  template <>
  inline constexpr auto Shapes<Entities::Callout> =
      std::tuple{
          Fields::Sequence{.Marker = "AcDbMLeader"},
          Fields::Since{
              .Release = Version::R2010,
              .Fields  = std::tuple{Fields::Field{
                  .Code   = 270,
                  .Member = &Entities::Callout::Version,
              }},
          },
          Fields::Literal{.Code = 300, .Text = "CONTEXT_DATA{"},
          Fields::Part{
              .Member = &Entities::Callout::Context,
              .Fields = Shapes<Entities::Note>,
          },
          Fields::Literal{.Code = 301, .Text = Fields::Closed},
          Fields::Field{.Code = 340, .Member = &Entities::Callout::Style},
          Fields::Field{.Code = 90, .Member = &Entities::Callout::Overrides},
          Fields::Field{.Code = 170, .Member = &Entities::Callout::Line},
          Fields::Field{.Code = 91, .Member = &Entities::Callout::Color},
          Fields::Field{.Code = 341, .Member = &Entities::Callout::Linetype},
          Fields::Field{.Code = 171, .Member = &Entities::Callout::Lineweight},
          Fields::Field{.Code = 290, .Member = &Entities::Callout::Landing},
          Fields::Field{.Code = 291, .Member = &Entities::Callout::Bent},
          Fields::Field{.Code = 41, .Member = &Entities::Callout::Length},
          Fields::Field{.Code = 342, .Member = &Entities::Callout::Arrow, .Fallback = Handle{}},
          Fields::Field{.Code = 42, .Member = &Entities::Callout::Size},
          Fields::Field{.Code = 172, .Member = &Entities::Callout::Content},
          Fields::Field{.Code = 343, .Member = &Entities::Callout::Lettering},
          Fields::Field{.Code = 173, .Member = &Entities::Callout::Left},
          Fields::Field{.Code = 95, .Member = &Entities::Callout::Right},
          Fields::Field{.Code = 174, .Member = &Entities::Callout::Angle},
          Fields::Field{.Code = 175, .Member = &Entities::Callout::Align},
          Fields::Field{.Code = 92, .Member = &Entities::Callout::Tint},
          Fields::Field{.Code = 292, .Member = &Entities::Callout::Framed},
          Fields::Field{.Code = 344, .Member = &Entities::Callout::Block, .Fallback = Handle{}},
          Fields::Field{.Code = 93, .Member = &Entities::Callout::Shade},
          Fields::Field{.Code = 10, .Member = &Entities::Callout::Sized},
          Fields::Field{.Code = 43, .Member = &Entities::Callout::Turned},
          Fields::Field{.Code = 176, .Member = &Entities::Callout::Connection},
          Fields::Field{.Code = 293, .Member = &Entities::Callout::Annotative},
          Fields::Run{
              .Opener = 94,
              .Member = &Entities::Callout::Tips,
              .Fields = Shapes<Entities::Tip>,
          },
          Fields::Run{
              .Opener = 330,
              .Member = &Entities::Callout::Labels,
              .Fields = Shapes<Entities::Label>,
          },
          Fields::Field{.Code = 294, .Member = &Entities::Callout::Negative},
          Fields::Field{.Code = 178, .Member = &Entities::Callout::Pinned},
          Fields::Field{.Code = 179, .Member = &Entities::Callout::Point},
          Fields::Field{.Code = 45, .Member = &Entities::Callout::Factor},
          Fields::Since{
              .Release = Version::R2010,
              .Fields =
                  std::tuple{
                      Fields::Field{
                          .Code   = 271,
                          .Member = &Entities::Callout::Attachment,
                      },
                      Fields::Field{.Code = 272, .Member = &Entities::Callout::Bottom},
                      Fields::Field{.Code = 273, .Member = &Entities::Callout::Top},
                  },
          },
          Fields::Since{
              .Release = Version::R2013,
              .Fields  = std::tuple{Fields::Field{
                  .Code   = 295,
                  .Member = &Entities::Callout::Extended,
              }},
          },
  };

  template <>
  inline constexpr auto Shapes<Entities::MultiLeader> = std::tuple{
      Entities::Part<Entities::MultiLeader>,
      Fields::Picture{.Member = &Entities::MultiLeader::Picture},
      Fields::Part{
          .Member = &Entities::MultiLeader::Callout,
          .Fields = Shapes<Entities::Callout>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Entities::MultiLeader> = "MULTILEADER";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_MLEADER_HPP */
