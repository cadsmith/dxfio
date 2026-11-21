#ifndef DXFIO_OBJECTS_SHAPES_TABLE_STYLE_HPP
#define DXFIO_OBJECTS_SHAPES_TABLE_STYLE_HPP

#include "fields/every.hpp"
#include "fields/field.hpp"
#include "fields/first.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/objects/table/style.hpp"
#include "dxfio/version.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
  namespace Objects::TableStyles
  {
    template <int At>
    inline constexpr auto Edge = std::tuple{
        Fields::Field<Border, int>{.Code = 274 + At, .Member = &Border::Lineweight},
        Fields::Field<Border, bool>{.Code = 284 + At, .Member = &Border::Visible},
        Fields::Field<Border, int>{.Code = 64 + At, .Member = &Border::Color},
    };
  } // namespace Objects::TableStyles

  template <>
  inline constexpr auto Shapes<Objects::TableStyles::RowStyle> =
      std::tuple{
          Fields::Field{.Code = 7, .Member = &Objects::TableStyles::RowStyle::Style},
          Fields::Field{.Code = 140, .Member = &Objects::TableStyles::RowStyle::Height},
          Fields::Field{.Code = 170, .Member = &Objects::TableStyles::RowStyle::Alignment},
          Fields::Field{.Code = 62, .Member = &Objects::TableStyles::RowStyle::Color},
          Fields::Field{.Code = 63, .Member = &Objects::TableStyles::RowStyle::Fill},
          Fields::Field{.Code = 283, .Member = &Objects::TableStyles::RowStyle::Filled},
          Fields::Since{
              .Release = Version::R2007,
              .Fields =
                  std::tuple{
                      Fields::Field{
                          .Code   = 90,
                          .Member = &Objects::TableStyles::RowStyle::Type,
                      },
                      Fields::Field{
                          .Code   = 91,
                          .Member = &Objects::TableStyles::RowStyle::Unit,
                      },
                      Fields::Field{
                          .Code   = 1,
                          .Member = &Objects::TableStyles::RowStyle::Format,
                      },
                  },
          },
          Fields::Part{
              .Member = &Objects::TableStyles::RowStyle::Top,
              .Fields = Objects::TableStyles::Edge<0>,
          },
          Fields::Part{
              .Member = &Objects::TableStyles::RowStyle::Middle,
              .Fields = Objects::TableStyles::Edge<1>,
          },
          Fields::Part{
              .Member = &Objects::TableStyles::RowStyle::Bottom,
              .Fields = Objects::TableStyles::Edge<2>,
          },
          Fields::Part{
              .Member = &Objects::TableStyles::RowStyle::Left,
              .Fields = Objects::TableStyles::Edge<3>,
          },
          Fields::Part{
              .Member = &Objects::TableStyles::RowStyle::Center,
              .Fields = Objects::TableStyles::Edge<4>,
          },
          Fields::Part{
              .Member = &Objects::TableStyles::RowStyle::Right,
              .Fields = Objects::TableStyles::Edge<5>,
          },
  };

  template <>
  inline constexpr auto Shapes<Objects::TableStyle> = std::tuple{
      Fields::Subclass{.Marker = "AcDbTableStyle"},
      Fields::Since{
          .Release = Version::R2010,
          .Fields  = std::tuple{Fields::First{
              .Code   = 280,
              .Fields = std::tuple{Fields::Field{
                  .Code   = 280,
                  .Member = &Objects::TableStyle::Revision,
              }},
          }},
      },
      Fields::Field{.Code = 3, .Member = &Objects::TableStyle::Description},
      Fields::Field{.Code = 70, .Member = &Objects::TableStyle::Flow},
      Fields::Field{.Code = 71, .Member = &Objects::TableStyle::Flags},
      Fields::Field{.Code = 40, .Member = &Objects::TableStyle::Horizontal},
      Fields::Field{.Code = 41, .Member = &Objects::TableStyle::Vertical},
      Fields::Field{.Code = 280, .Member = &Objects::TableStyle::Untitled},
      Fields::Field{.Code = 281, .Member = &Objects::TableStyle::Headless},
      Fields::Every{
          .Opener = 7,
          .Member = &Objects::TableStyle::Cells,
          .Fields = Shapes<Objects::TableStyles::RowStyle>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Objects::TableStyle> = "TABLESTYLE";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_TABLE_STYLE_HPP */
