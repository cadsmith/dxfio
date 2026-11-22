#ifndef DXFIO_EMPTY_SPACE_HPP
#define DXFIO_EMPTY_SPACE_HPP

#include "blocks/io.hpp"
#include "seed.hpp"

#include "dxfio/api.hpp"
#include "dxfio/blocks/definition.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/objects/layout.hpp"

#include <string>

namespace DxfIO::Empty
{
  struct DXFIO_API Space
  {
    Handle Record{};
    bool   Paper{};
  };

  [[nodiscard]] DXFIO_API auto Layout(std::string name, Handle self, Handle record)
      -> Objects::Layout;

  [[nodiscard]] DXFIO_API auto Block(std::string name, Space space, Seed &handles)
      -> Blocks::Definition;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_SPACE_HPP */
