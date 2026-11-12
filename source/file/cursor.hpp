#ifndef DXFIO_FILE_CURSOR_HPP
#define DXFIO_FILE_CURSOR_HPP

#include "file/record.hpp"
#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <cstddef>
#include <expected>
#include <span>
#include <string_view>
#include <vector>

namespace DxfIO::File
{
  class DXFIO_API Cursor
  {
  public:
    explicit Cursor(std::span<const Tags::Tag> all);

    [[nodiscard]] auto Done() const -> bool;

    auto Take() -> std::expected<Record, Failure>;

    auto Take(std::string_view name) -> std::expected<Record, Failure>;

    auto Until(std::string_view end) -> std::expected<std::vector<Record>, Failure>;

  private:
    [[nodiscard]] auto Rest() const -> std::span<const Tags::Tag>;
    [[nodiscard]] auto Line() const -> std::size_t;

    std::span<const Tags::Tag> m_All;
    std::size_t                m_At{};
  };
} // namespace DxfIO::File

#endif /* DXFIO_FILE_CURSOR_HPP */
