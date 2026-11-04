#ifndef DXFIO_TESTS_DXF_AUTOCAD_HPP
#define DXFIO_TESTS_DXF_AUTOCAD_HPP

#include "tags/codes.hpp"

#include <string>
#include <string_view>

namespace DxfIO
{
inline auto Around(std::string_view text, std::string_view marker) -> std::string
{
  constexpr std::string_view opener = "\r\n  0\r\n";
  const std::size_t at = text.find(marker);
  const std::size_t from = at == std::string_view::npos ? at : text.rfind(opener, at);
  const std::size_t to = from == std::string_view::npos ? from : text.find(opener, at + 1);
  if (to == std::string_view::npos)
  {
    return {};
  }
  const std::size_t start = from + 2;
  return std::string{text.substr(start, to + 2 - start)};
}

inline auto Autocad(std::string_view text, std::string_view self) -> std::string
{
  const std::string handle = "\r\n" + std::string{self} + "\r\n";
  const std::string found = Around(text, "\r\n  5" + handle);
  return found.empty() ? Around(text, "\r\n105" + handle) : found;
}

inline auto Declared(std::string_view text, std::string_view name) -> std::string
{
  return Around(text, "CLASS\r\n  1\r\n" + std::string{name} + "\r\n");
}

inline auto Masked(std::string_view text) -> std::string
{
  std::string out;
  bool skipping = false;
  while (!text.empty())
  {
    const std::size_t first = text.find("\r\n");
    const std::size_t second = text.find("\r\n", first + 2);
    const std::string line{text.substr(0, first)};
    const std::string value{text.substr(first + 2, second - first - 2)};
    text.remove_prefix(second + 2);
    const int code = std::stoi(line);
    if (code >= 1000)
    {
      break;
    }
    if (skipping || (code == 102 && value == "{ACAD_XDICTIONARY"))
    {
      skipping = !(code == 102 && value == "}");
      continue;
    }
    const bool pointer = Tags::Codes::Of(code) == Tags::Codes::Type::Handle;
    out += line + "\r\n" + (pointer ? "*" : value) + "\r\n";
  }
  return out;
}
} // namespace DxfIO

#endif /* DXFIO_TESTS_DXF_AUTOCAD_HPP */
