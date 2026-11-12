#ifndef DXFIO_OBJECTS_DICTIONARY_HPP
#define DXFIO_OBJECTS_DICTIONARY_HPP

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/handle.hpp"

#include <string>
#include <vector>

namespace DxfIO::Objects
{
  /**
   *  One entry of a dictionary: a name and the object it stands for.
   */
  struct DXFIO_API Entry
  {
    /**
     *  The name the object is found by. Group 3.
     */
    std::string Name{};
    /**
     *  The object, when the dictionary does not own it hard. Group 350.
     */
    Handle Soft{};
    /**
     *  The object, when the dictionary owns it hard. Group 360.
     */
    Handle Hard{};

    auto operator==(const Entry &) const -> bool = default;
  };

  /**
   *  A DICTIONARY: objects found by name, such as the drawing's layouts, groups or styles.
   */
  struct DXFIO_API Dictionary
  {
    /**
     *  The handle, owner, reactors and extended data.
     */
    DxfIO::Base Base{};
    /**
     *  Whether the dictionary owns its entries hard. Group 280, from R2000.
     */
    bool Hard{};
    /**
     *  What cloning does with a name already taken: 0 nothing, 1 keeps the entry there, 2 takes the
     *  clone, 3 to 5 rename it. Group 281, from R2000.
     */
    int Cloning{1};
    /**
     *  The entries, in the order the file gives them.
     */
    std::vector<Objects::Entry> Entries{};

    auto operator==(const Dictionary &) const -> bool = default;
  };
} // namespace DxfIO::Objects

#endif /* DXFIO_OBJECTS_DICTIONARY_HPP */
