#ifndef DXFIO_TAGS_TAGS_HPP
#define DXFIO_TAGS_TAGS_HPP

#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <concepts>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Tags
{
// Every tag of a file, ASCII or binary, up to its EOF, with the comments left out; a group whose value does not
// parse, and whatever follows the EOF, are left out and told to issues, when given. Fails where the file cannot be
// read on.
[[nodiscard]] DXFIO_API auto All(std::string_view bytes, std::vector<Issue> *issues = nullptr)
    -> std::expected<std::vector<Tag>, Failure>;

template <class BufferT>
  requires std::same_as<BufferT, std::string>
auto All(BufferT &&dying, std::vector<Issue> *issues = nullptr) -> std::expected<std::vector<Tag>, Failure> = delete;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_TAGS_HPP */
