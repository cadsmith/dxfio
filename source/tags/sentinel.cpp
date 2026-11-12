#include "tags/sentinel.hpp"

namespace DxfIO::Tags
{
  auto Sentinel() -> std::string_view
  {
    using namespace std::string_view_literals;
    return "AutoCAD Binary DXF\r\n\x1a\0"sv;
  }

  auto Marked(std::string_view bytes) -> bool
  {
    return bytes.starts_with(Sentinel());
  }
} // namespace DxfIO::Tags
