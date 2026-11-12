#ifndef DXFIO_ENCODING_CODEPAGE_HPP
#define DXFIO_ENCODING_CODEPAGE_HPP

#include "dxfio/api.hpp"

#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <string_view>

namespace DxfIO::Encoding
{
  inline constexpr std::size_t Half = 128;

  struct DXFIO_API Codepage
  {
    std::string_view                  Name;
    const std::array<char16_t, Half> *High{nullptr};
    std::span<const char16_t>         Pairs;
    std::size_t                       Index{};
  };

  using Twin = std::array<char, 2>;

  [[nodiscard]] DXFIO_API auto Pages() -> std::span<const Codepage>;

  [[nodiscard]] DXFIO_API auto Find(std::string_view name) -> const Codepage *;

  [[nodiscard]] DXFIO_API auto Standard() -> const Codepage &;

  [[nodiscard]] DXFIO_API auto Decoded(const Codepage &page, unsigned char byte) -> char32_t;

  [[nodiscard]] DXFIO_API auto Encoded(const Codepage &page, char32_t point) -> std::optional<char>;

  [[nodiscard]] DXFIO_API auto Paired(const Codepage &page, unsigned char lead, unsigned char trail)
      -> char32_t;

  [[nodiscard]] DXFIO_API auto Doubled(const Codepage &page, char32_t point) -> std::optional<Twin>;
} // namespace DxfIO::Encoding

#endif /* DXFIO_ENCODING_CODEPAGE_HPP */
