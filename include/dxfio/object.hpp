#ifndef DXFIO_OBJECT_HPP
#define DXFIO_OBJECT_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/objects/datatable.hpp"
#include "dxfio/objects/defaulted.hpp"
#include "dxfio/objects/dictionary.hpp"
#include "dxfio/objects/dictionaryvar.hpp"
#include "dxfio/objects/group.hpp"
#include "dxfio/objects/image/definition.hpp"
#include "dxfio/objects/layout.hpp"
#include "dxfio/objects/material.hpp"
#include "dxfio/objects/mleader/style.hpp"
#include "dxfio/objects/mline/style.hpp"
#include "dxfio/objects/placeholder.hpp"
#include "dxfio/objects/plotsettings.hpp"
#include "dxfio/objects/scale.hpp"
#include "dxfio/objects/sortents/table.hpp"
#include "dxfio/objects/table/style.hpp"
#include "dxfio/objects/underlay/definition.hpp"
#include "dxfio/objects/visualstyle.hpp"
#include "dxfio/objects/wipeout/variables.hpp"
#include "dxfio/objects/xrecord.hpp"
#include "dxfio/unknown.hpp"

#include <string_view>
#include <variant>

namespace DxfIO
{
/// An object of the OBJECTS section: one of the kinds DXF defines, or an Unknown kept as it came.
using Object =
    std::variant<Objects::Dictionary, Objects::DictionaryWithDefault, Objects::Group, Objects::ImageDef,
                 Objects::ImageDefReactor, Objects::RasterVariables, Objects::Layout, Objects::Material,
                 Objects::MLeaderStyle, Objects::MLineStyle, Objects::Placeholder, Objects::Scale,
                 Objects::SortEntsTable, Objects::TableStyle, Objects::DictionaryVar, Objects::XRecord,
                 Objects::PlotSettings, Objects::WipeoutVariables, Objects::PdfDefinition, Objects::DwfDefinition,
                 Objects::DgnDefinition, Objects::VisualStyle, Objects::DataTable, Objects::AcDbDataTable, Unknown>;

/// The DXF name of an object, such as "DICTIONARY", or the name an unknown object came in with.
[[nodiscard]] DXFIO_API auto TypeOf(const Object &one) -> std::string_view;

/// The handle, owner, reactors and extended data every object carries.
[[nodiscard]] DXFIO_API auto BaseOf(const Object &one) -> const Base &;

/// The handle, owner, reactors and extended data every object carries.
[[nodiscard]] DXFIO_API auto BaseOf(Object &one) -> Base &;
} // namespace DxfIO

#endif /* DXFIO_OBJECT_HPP */
