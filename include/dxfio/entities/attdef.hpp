#ifndef DXFIO_ENTITIES_ATTDEF_HPP
#define DXFIO_ENTITIES_ATTDEF_HPP

#include "dxfio/api.hpp"
#include "dxfio/boxed.hpp"
#include "dxfio/entities/lettering.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/symbol.hpp"

#include <string>

namespace DxfIO::Entities
{
/// An ATTDEF: an attribute a block defines, which each INSERT of the block gives a value.
struct DXFIO_API Attdef
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};
  /// The default value, where it is drawn, and in which style, kept on the heap and reached with ->.
  Boxed<Entities::Lettering> Lettering;
  /// The version of the attribute's layout. The first group 280, from R2010.
  int Revision{};
  /// The prompt shown when the block is inserted. Group 3.
  std::string Prompt;
  /// The tag that names the attribute in its block. Group 2.
  Symbol Tag;
  /// 1 invisible, 2 constant, 4 verified on entry, 8 preset. Group 70.
  int Flags{};
  /// The field length, which AutoCAD does not use. Group 73.
  int Length{};
  /// The vertical justification: 0 baseline, 1 bottom, 2 middle, 3 top. Group 74.
  int Vertical{};
  /// Whether the attribute is locked to its place in the block. The last group 280, from R2007.
  bool Locked{};

  auto operator==(const Attdef &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_ATTDEF_HPP */
