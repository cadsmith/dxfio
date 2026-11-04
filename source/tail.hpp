#ifndef DXFIO_TAIL_HPP
#define DXFIO_TAIL_HPP

#include "fields/context.hpp"
#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO
{
[[nodiscard]] DXFIO_API auto Trailing(std::span<const Tags::Tag> body) -> std::size_t;

DXFIO_API void Tail(std::span<const Tags::Tag> groups, Base &into, const Fields::Reading &with);

[[nodiscard]] DXFIO_API auto Tail(Fields::Writing &to, const Base &from) -> std::expected<void, Failure>;
} // namespace DxfIO

#endif /* DXFIO_TAIL_HPP */
