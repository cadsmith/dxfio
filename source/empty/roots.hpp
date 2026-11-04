#ifndef DXFIO_EMPTY_ROOTS_HPP
#define DXFIO_EMPTY_ROOTS_HPP

#include "seed.hpp"

#include "dxfio/handle.hpp"

namespace DxfIO::Empty
{
struct Spaces
{
  Handle Model{};
  Handle Paper{};
};

struct Materials
{
  Handle Byblock{};
  Handle Bylayer{};
  Handle Global{};
};

struct Roots
{
  Handle Dictionary{};
  Spaces Records{};
  Spaces Layouts{};
  Empty::Materials Materials{};
  Handle Normal{};
  Handle Linetype{};
  Handle Style{};
};

auto Taken(Seed &handles) -> Roots;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_ROOTS_HPP */
