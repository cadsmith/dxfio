#ifndef DXFIO_OBJECTS_DICTIONARYVAR_HPP
#define DXFIO_OBJECTS_DICTIONARYVAR_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"

#include <string>

namespace DxfIO::Objects
{
/// A DICTIONARYVAR: a system variable kept in the drawing's variable dictionary, named by its entry there.
struct DXFIO_API DictionaryVar
{
  /// The handle, owner, reactors and extended data.
  DxfIO::Base Base{};
  /// The schema number of the object, 0 as AutoCAD writes it. Group 280.
  int Schema{};
  /// The variable's value, as text. Group 1.
  std::string Value;

  auto operator==(const DictionaryVar &) const -> bool = default;
};
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_DICTIONARYVAR_HPP */
