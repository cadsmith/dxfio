#ifndef DXFIO_OBJECTS_SHAPES_SORTENTS_TABLE_HPP
#define DXFIO_OBJECTS_SHAPES_SORTENTS_TABLE_HPP

#include "fields/every.hpp"
#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/optional.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "shapes.hpp"

#include "dxfio/handle.hpp"
#include "dxfio/objects/sortents/table.hpp"
#include "dxfio/version.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Objects::Sortents::Entry> = std::tuple{
    Fields::Field{.Code = 331, .Member = &Objects::Sortents::Entry::Entity},
    Fields::Field{.Code = 5, .Member = &Objects::Sortents::Entry::Key},
};

template <>
inline constexpr auto Shapes<Objects::SortEntsTable> = std::tuple{
    Fields::Subclass{.Marker = "AcDbSortentsTable"},
    Fields::Since{
        .Release = Version::R13,
        .Fields =
            std::tuple{
                Fields::Field{.Code = 330, .Member = &Objects::SortEntsTable::Block, .Fallback = Handle{}},
                Fields::Every{
                    .Opener = 331,
                    .Member = &Objects::SortEntsTable::Entries,
                    .Fields = Shapes<Objects::Sortents::Entry>,
                },
            },
    },
};

template <>
inline constexpr std::string_view Type<Objects::SortEntsTable> = "SORTENTSTABLE";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_SORTENTS_TABLE_HPP */
