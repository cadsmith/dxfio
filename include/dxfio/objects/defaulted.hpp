#ifndef DXFIO_OBJECTS_DEFAULTED_HPP
#define DXFIO_OBJECTS_DEFAULTED_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/objects/dictionary.hpp"

namespace DxfIO::Objects
{
  /**
   *  An ACDBDICTIONARYWDFLT: a dictionary with an entry to fall back on, such as the plot style
   *  names with Normal.
   */
  struct DXFIO_API DictionaryWithDefault
  {
    /**
     *  The entries and flags it holds as any dictionary does.
     */
    Objects::Dictionary Dictionary{};
    /**
     *  The object taken when a name is not found. Group 340.
     */
    Handle Fallback{};

    auto operator==(const DictionaryWithDefault &) const -> bool = default;
  };
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_DEFAULTED_HPP */
