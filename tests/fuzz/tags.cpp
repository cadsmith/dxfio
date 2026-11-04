#include "tags/tags.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>

extern "C" auto LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) -> int
{
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view bytes{reinterpret_cast<const char *>(data), size};
  static_cast<void>(DxfIO::Tags::All(bytes));
  return 0;
}
