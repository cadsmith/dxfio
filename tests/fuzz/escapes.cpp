#include "encoding/codepage.hpp"
#include "encoding/encoding.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>

namespace
{
void Check(const DxfIO::Encoding::Scheme &with, std::string_view bytes)
{
  const std::string text = Decode(DxfIO::Encoding::Utf8(), bytes);
  const std::string once = Decode(with, Encode(with, text));
  const std::string written = Encode(with, once);
  if (written.find_first_of("\r\n") != std::string::npos || Decode(with, written) != once)
  {
    std::abort();
  }
}
} // namespace

extern "C" auto LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) -> int
{
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const std::string_view bytes{reinterpret_cast<const char *>(data), size};
  Check(DxfIO::Encoding::Utf8(), bytes);
  for (const DxfIO::Encoding::Codepage &page : DxfIO::Encoding::Pages())
  {
    Check(DxfIO::Encoding::Paged(page), bytes);
  }
  return 0;
}
