#ifndef DXFIO_TAGS_ASCII_READER_HPP
#define DXFIO_TAGS_ASCII_READER_HPP

#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <cstddef>
#include <expected>
#include <string_view>

namespace DxfIO::Tags::Ascii
{
class DXFIO_API Reader
{
public:
  explicit Reader(std::string_view text);

  [[nodiscard]] auto Done() const -> bool;

  auto Next() -> std::expected<Tag, Failure>;

private:
  auto Line() -> std::string_view;

  std::string_view m_Text;
  std::size_t m_At{};
  std::size_t m_Line{};
};
} // namespace DxfIO::Tags::Ascii

#endif /* DXFIO_TAGS_ASCII_READER_HPP */
