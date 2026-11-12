#ifndef DXFIO_ASCII_HPP
#define DXFIO_ASCII_HPP

#include <algorithm>
#include <string_view>

namespace DxfIO::Ascii
{
  constexpr auto Upper(char letter) -> char
  {
    return letter >= 'a' && letter <= 'z' ? static_cast<char>(letter - 'a' + 'A') : letter;
  }

  constexpr auto Lower(char letter) -> char
  {
    constexpr char Gap = 'a' - 'A';
    return letter >= 'A' && letter <= 'Z' ? static_cast<char>(letter + Gap) : letter;
  }

  constexpr auto Same(std::string_view left, std::string_view right) -> bool
  {
    return std::ranges::equal(left, right, {}, Upper, Upper);
  }
} // namespace DxfIO::Ascii

#endif /* DXFIO_ASCII_HPP */
