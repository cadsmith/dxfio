#include "oracle.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>

extern "C" auto LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) -> int
{
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view bytes{reinterpret_cast<const char *>(data), size};
  const std::string      found = DxfIO::Broken(bytes);
  if (!found.empty())
  {
    (void)std::fputs(found.c_str(), stderr);
    std::abort();
  }
  return 0;
}
