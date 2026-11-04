#ifndef DXFIO_TAGS_BINARY_READER_HPP
#define DXFIO_TAGS_BINARY_READER_HPP

#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>
#include <optional>
#include <string_view>

namespace DxfIO::Tags::Binary
{
class DXFIO_API Reader
{
public:
  explicit Reader(std::string_view bytes);

  [[nodiscard]] auto Done() const -> bool;

  auto Next() -> std::expected<Tag, Failure>;

private:
  auto Code() -> std::optional<int>;

  std::string_view m_Rest;
  bool m_Wide{};
};
} // namespace DxfIO::Tags::Binary

#endif /* DXFIO_TAGS_BINARY_READER_HPP */
