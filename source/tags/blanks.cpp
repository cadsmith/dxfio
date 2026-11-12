#include "blanks.hpp"

namespace DxfIO::Tags
{
  auto Trimmed(std::string_view text) -> std::string_view
  {
    constexpr std::string_view Blanks = " \t";
    const std::size_t          first  = text.find_first_not_of(Blanks);
    if (first == std::string_view::npos)
    {
      return {};
    }
    return text.substr(first, text.find_last_not_of(Blanks) - first + 1);
  }
} // namespace DxfIO::Tags
