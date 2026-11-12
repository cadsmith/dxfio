#ifndef DXFIO_TAGS_DECIMAL_HPP
#define DXFIO_TAGS_DECIMAL_HPP

#include "dxfio/api.hpp"

#include <optional>
#include <string_view>

namespace DxfIO::Tags
{
  /**
   *  The double nearest the decimal word spells, such as -1.5 or 2.5E+03; nothing for a word with
   *  blanks, a plus, an infinity, a NaN, or a value beyond a double.
   */
  [[nodiscard]] DXFIO_API auto Decimal(std::string_view word) -> std::optional<double>;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_DECIMAL_HPP */
