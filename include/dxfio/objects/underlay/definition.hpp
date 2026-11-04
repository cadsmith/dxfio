#ifndef DXFIO_OBJECTS_UNDERLAY_DEFINITION_HPP
#define DXFIO_OBJECTS_UNDERLAY_DEFINITION_HPP

#include "dxfio/base.hpp"
#include "dxfio/entities/overlay.hpp"

#include <string>

namespace DxfIO::Objects
{
/// An underlay definition: the PDF, DWF or DGN file the drawing's underlays of that kind show.
template <Entities::Overlay KindV>
struct UnderlayDefinition
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// The path of the file. Group 1.
  std::string File;
  /// The part of the file shown: a PDF's page, a DWF's sheet or a DGN's model. Group 2.
  std::string Name;

  auto operator==(const UnderlayDefinition &) const -> bool = default;
};

/// A PDFDEFINITION: a PDF file and the page shown.
using PdfDefinition = UnderlayDefinition<Entities::Overlay::Pdf>;
/// A DWFDEFINITION: a DWF file and the sheet shown.
using DwfDefinition = UnderlayDefinition<Entities::Overlay::Dwf>;
/// A DGNDEFINITION: a DGN file and the model shown.
using DgnDefinition = UnderlayDefinition<Entities::Overlay::Dgn>;
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_UNDERLAY_DEFINITION_HPP */
