#ifndef DXFIO_ENCODING_CARETS_HPP
#define DXFIO_ENCODING_CARETS_HPP

#include "dxfio/api.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace DxfIO::Encoding
{
[[nodiscard]] DXFIO_API auto Caret(std::string_view raw) -> std::optional<char>;

DXFIO_API void Careted(std::string &out, char letter);
} // namespace DxfIO::Encoding

#endif /* DXFIO_ENCODING_CARETS_HPP */
