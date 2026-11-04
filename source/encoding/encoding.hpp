#ifndef DXFIO_ENCODING_ENCODING_HPP
#define DXFIO_ENCODING_ENCODING_HPP

#include "encoding/codepage.hpp"

#include "dxfio/api.hpp"
#include "dxfio/version.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Encoding
{
struct DXFIO_API Scheme
{
  const Codepage *Page{nullptr};
  std::string (*In)(std::string_view raw, const Codepage *page){nullptr};
  std::string (*Out)(std::string_view text, const Codepage *page){nullptr};
};

[[nodiscard]] DXFIO_API auto Utf8() -> Scheme;

[[nodiscard]] DXFIO_API auto Paged(const Codepage &page) -> Scheme;

[[nodiscard]] DXFIO_API auto Opaque() -> Scheme;

[[nodiscard]] DXFIO_API auto Of(Version release, const Codepage &page) -> Scheme;

[[nodiscard]] DXFIO_API auto Decode(const Scheme &with, std::string_view raw) -> std::string;

[[nodiscard]] DXFIO_API auto Encode(const Scheme &with, std::string_view text) -> std::string;
} // namespace DxfIO::Encoding

#endif /* DXFIO_ENCODING_ENCODING_HPP */
