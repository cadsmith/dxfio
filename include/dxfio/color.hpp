#ifndef DXFIO_COLOR_HPP
#define DXFIO_COLOR_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"

#include <cstdint>
#include <optional>

namespace DxfIO
{
  /**
   *  How a color is given, as AutoCAD's color method byte stores it.
   */
  enum class ColorMethod : std::uint8_t
  {
    ByLayer = 0xC0,
    ByBlock = 0xC1,
    Rgb     = 0xC2,
    Index   = 0xC3, /**< An entry of the AutoCAD palette. */
  };

  /**
   *  How a transparency is given, as the top byte of group 440 stores it.
   */
  enum class TransparencyMethod : std::uint8_t
  {
    ByLayer = 0,
    ByBlock = 1,
    Alpha   = 2, /**< The low byte holds the alpha: 0 clear, 255 opaque. */
  };

  /**
   *  Rgb when the entity has a true color; otherwise ByBlock for color 0, ByLayer for 256 and Index
   *  for the rest.
   */
  [[nodiscard]] DXFIO_API auto MethodOf(const Entities::Record &record) -> ColorMethod;

  /**
   *  Red, green and blue as 0xRRGGBB, for an entity with a true color; empty for any other.
   */
  [[nodiscard]] DXFIO_API auto RgbOf(const Entities::Record &record)
      -> std::optional<std::uint32_t>;

  /**
   *  Empty for an entity without a transparency of its own, and for one whose top byte names no
   *  method.
   */
  [[nodiscard]] DXFIO_API auto TransparencyMethodOf(const Entities::Record &record)
      -> std::optional<TransparencyMethod>;

  /**
   *  The alpha, 0 clear to 255 opaque, for a transparency whose method is Alpha; empty for any
   *  other.
   */
  [[nodiscard]] DXFIO_API auto AlphaOf(const Entities::Record &record)
      -> std::optional<std::uint8_t>;
} // namespace DxfIO

#endif /* DXFIO_COLOR_HPP */
