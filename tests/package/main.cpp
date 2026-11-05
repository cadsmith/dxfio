#include "dxfio/dxfio.hpp"

#include <cstdio>

auto main(int count, char **paths) -> int
{
  if (count != 2)
  {
    return 2;
  }
  const auto loaded = DxfIO::LoadFile(paths[1]);
  if (!loaded)
  {
    std::puts(loaded.error().What.c_str());
    return 1;
  }
  std::printf("%zu entities\n", loaded->Document.Entities.size());
  return 0;
}
