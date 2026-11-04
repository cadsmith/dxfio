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
namespace Objects::Mleader
{
inline constexpr auto Lines = std::tuple{
    Fields::Field{.Code = 172, .Member = &Leader::Order},    Fields::Field{.Code = 90, .Member = &Leader::Points},
    Fields::Field{.Code = 40, .Member = &Leader::First},     Fields::Field{.Code = 41, .Member = &Leader::Second},
    Fields::Field{.Code = 173, .Member = &Leader::Type},     Fields::Field{.Code = 91, .Member = &Leader::Color},
    Fields::Field{.Code = 340, .Member = &Leader::Linetype}, Fields::Field{.Code = 92, .Member = &Leader::Lineweight},
    Fields::Field{.Code = 290, .Member = &Leader::Landing},  Fields::Field{.Code = 42, .Member = &Leader::Gap},
    Fields::Field{.Code = 291, .Member = &Leader::Dogleg},   Fields::Field{.Code = 43, .Member = &Leader::Length},
};

inline constexpr auto Heads = std::tuple{
    Fields::Field{.Code = 341, .Member = &Leader::Arrow, .Fallback = Handle{}},
    Fields::Field{.Code = 44, .Member = &Leader::Size},
};

inline constexpr auto Words = std::tuple{
    Fields::Field{.Code = 300, .Member = &Text::Content}, Fields::Field{.Code = 342, .Member = &Text::Style},
    Fields::Field{.Code = 174, .Member = &Text::Left},    Fields::Field{.Code = 178, .Member = &Text::Right},
    Fields::Field{.Code = 175, .Member = &Text::Angle},   Fields::Field{.Code = 176, .Member = &Text::Alignment},
    Fields::Field{.Code = 93, .Member = &Text::Color},    Fields::Field{.Code = 45, .Member = &Text::Height},
    Fields::Field{.Code = 292, .Member = &Text::Frame},   Fields::Field{.Code = 297, .Member = &Text::Flush},
    Fields::Field{.Code = 46, .Member = &Text::Space},
};

inline constexpr auto Attachments = std::tuple{
    Fields::Field{.Code = 271, .Member = &Text::Direction},
    Fields::Field{.Code = 272, .Member = &Text::Bottom},
    Fields::Field{.Code = 273, .Member = &Text::Top},
};

inline constexpr auto Shown = std::tuple{
    Fields::Field{.Code = 343, .Member = &Block::Record, .Fallback = Handle{}},
    Fields::Field{.Code = 94, .Member = &Block::Color},
    Fields::Part{
        .Member = &Block::Scale,
        .Fields =
            std::tuple{
                Fields::Field<Vector, double>{.Code = 47, .Member = &Vector::X},
                Fields::Field<Vector, double>{.Code = 49, .Member = &Vector::Y},
                Fields::Field<Vector, double>{
                    .Code = 140,
                    .Member = &Vector::Z,
                },
            },
    },
    Fields::Field{.Code = 293, .Member = &Block::Scaled},
    Fields::Field{.Code = 141, .Member = &Block::Rotation},
    Fields::Field{.Code = 294, .Member = &Block::Rotated},
    Fields::Field{.Code = 177, .Member = &Block::Connection},
};
} // namespace Objects::Mleader

template <>
inline constexpr auto Shapes<Objects::MLeaderStyle> = std::tuple{
    Fields::Subclass{.Marker = "AcDbMLeaderStyle"},
    Fields::Since{
        .Release = Version::R2010,
        .Fields = std::tuple{Fields::Field{
            .Code = 179,
            .Member = &Objects::MLeaderStyle::Revision,
        }},
    },
    Fields::Field{.Code = 170, .Member = &Objects::MLeaderStyle::Content},
    Fields::Field{.Code = 171, .Member = &Objects::MLeaderStyle::Order},
    Fields::Part{
        .Member = &Objects::MLeaderStyle::Leader,
        .Fields = Objects::Mleader::Lines,
    },
    Fields::Field{.Code = 3, .Member = &Objects::MLeaderStyle::Description},
    Fields::Part{
        .Member = &Objects::MLeaderStyle::Leader,
        .Fields = Objects::Mleader::Heads,
    },
    Fields::Part{
        .Member = &Objects::MLeaderStyle::Text,
        .Fields = Objects::Mleader::Words,
    },
    Fields::Part{
        .Member = &Objects::MLeaderStyle::Block,
        .Fields = Objects::Mleader::Shown,
    },
    Fields::Field{.Code = 142, .Member = &Objects::MLeaderStyle::Scale},
    Fields::Field{.Code = 295, .Member = &Objects::MLeaderStyle::Overwrite},
    Fields::Field{.Code = 296, .Member = &Objects::MLeaderStyle::Annotative},
    Fields::Field{.Code = 143, .Member = &Objects::MLeaderStyle::Breaks},
    Fields::Since{
        .Release = Version::R2010,
        .Fields = std::tuple{Fields::Part{
            .Member = &Objects::MLeaderStyle::Text,
            .Fields = Objects::Mleader::Attachments,
        }},
    },
    Fields::Since{
        .Release = Version::R2013,
        .Fields = std::tuple{Fields::Field{
            .Code = 298,
            .Member = &Objects::MLeaderStyle::Spare,
        }},
    },
};

template <>
inline constexpr std::string_view Type<Objects::MLeaderStyle> = "MLEADERSTYLE";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_MLEADER_STYLE_HPP */
