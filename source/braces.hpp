#ifndef DXFIO_BRACES_HPP
#define DXFIO_BRACES_HPP

#include "fields/context.hpp"
#include "tags/tag.hpp"

#include "dxfio/base.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <cstddef>
#include <expected>
#include <span>
#include <string_view>
#include <vector>

namespace DxfIO
{
inline constexpr int Reactor = 330;

inline constexpr std::string_view Reactors = "ACAD_REACTORS";

inline constexpr std::string_view Extension = "ACAD_XDICTIONARY";

auto Opens(const Tags::Tag &one) -> bool;

auto Braced(std::span<const Tags::Tag> body, Base &into, const Fields::Reading &with) -> std::size_t;

auto Braces(Fields::Writing &to, const Base &from) -> std::expected<void, Failure>;
} // namespace DxfIO

#endif /* DXFIO_BRACES_HPP */
