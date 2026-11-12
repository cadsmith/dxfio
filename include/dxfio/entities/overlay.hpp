#ifndef DXFIO_ENTITIES_OVERLAY_HPP
#define DXFIO_ENTITIES_OVERLAY_HPP

#include <cstdint>

namespace DxfIO::Entities
{
  /**
   *  The kind of file an underlay shows.
   */
  enum class Overlay : std::uint8_t
  {
    /**
     *  A PDF file, for a PDFUNDERLAY.
     */
    Pdf,
    /**
     *  A DWF file, for a DWFUNDERLAY.
     */
    Dwf,
    /**
     *  A MicroStation DGN file, for a DGNUNDERLAY.
     */
    Dgn,
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_OVERLAY_HPP */
