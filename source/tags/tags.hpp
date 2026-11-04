#ifndef DXFIO_TAGS_TAGS_HPP
#define DXFIO_TAGS_TAGS_HPP

#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <concepts>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Tags
{
[[nodiscard]] DXFIO_API auto All(std::string_view bytes) -> std::expected<std::vector<Tag>, Failure>;

template <class BufferT>
  requires std::same_as<BufferT, std::string>
auto All(BufferT &&dying) -> std::expected<std::vector<Tag>, Failure> = delete;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_TAGS_HPP */
