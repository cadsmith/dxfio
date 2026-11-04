#ifndef DXFIO_TAGS_TAG_HPP
#define DXFIO_TAGS_TAG_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <variant>

namespace DxfIO::Tags
{
struct DXFIO_API Chunk
{
  std::string_view Bytes;

  auto operator==(const Chunk &) const -> bool = default;
};

using Content = std::variant<std::string_view, std::int64_t, double, bool, Chunk>;

struct DXFIO_API Tag
{
  int Code{};
  Content Value;
  std::size_t Line{};

  auto operator==(const Tag &other) const -> bool;
};

[[nodiscard]] DXFIO_API auto Text(const Tag &one) -> std::string_view;

[[nodiscard]] DXFIO_API auto Holds(const Tag &one, int code, std::string_view value) -> bool;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_TAG_HPP */
