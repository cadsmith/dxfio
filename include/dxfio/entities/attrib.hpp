#ifndef DXFIO_ENTITIES_ATTRIB_HPP
#define DXFIO_ENTITIES_ATTRIB_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/lettering.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/symbol.hpp"

namespace DxfIO::Entities
{
/// An ATTRIB: the value an INSERT gives one attribute of its block.
struct DXFIO_API Attrib
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The value, where it is drawn, and in which style.
  Entities::Lettering Lettering{};
  /// The version of the attribute's layout. The first group 280, from R2010.
  int Revision{};
  /// The tag that names the attribute in its block. Group 2.
  Symbol Tag{};
  /// 1 invisible, 2 constant, 4 verified on entry, 8 preset. Group 70.
  int Flags{};
  /// The field length, which AutoCAD does not use. Group 73.
  int Length{};
  /// The vertical justification: 0 baseline, 1 bottom, 2 middle, 3 top. Group 74.
  int Vertical{};
  /// Whether the attribute is locked to its place in the block. The last group 280, from R2007.
  bool Locked{};

  auto operator==(const Attrib &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_ATTRIB_HPP */
