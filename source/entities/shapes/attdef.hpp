#ifndef DXFIO_ENTITIES_SHAPES_ATTDEF_HPP
#define DXFIO_ENTITIES_SHAPES_ATTDEF_HPP

#include "attribute.hpp"
#include "fields/field.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "lettering.hpp"
#include "record.hpp"
#include "shapes.hpp"

#include "dxfio/entities/attdef.hpp"

#include <string_view>
#include <tuple>

namespace DxfIO
{
template <>
inline constexpr auto Shapes<Entities::Attdef> = std::tuple_cat(
    std::tuple{
        Entities::Part<Entities::Attdef>,
        Entities::Lettered<Entities::Attdef>,
        Fields::Subclass{.Marker = "AcDbAttributeDefinition"},
        Entities::Revised<Entities::Attdef>,
        Fields::Field{.Code = 3, .Member = &Entities::Attdef::Prompt},
    },
    Entities::Labeled<Entities::Attdef>);

template <>
inline constexpr std::string_view Type<Entities::Attdef> = "ATTDEF";
} // namespace DxfIO

#endif /* DXFIO_ENTITIES_SHAPES_ATTDEF_HPP */
