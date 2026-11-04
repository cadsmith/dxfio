#ifndef DXFIO_TAGS_TYPED_HPP
#define DXFIO_TAGS_TYPED_HPP

#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <expected>

namespace DxfIO::Tags
{
// The value as its group holds it, for either form to write: a whole number within the group's width (16 bit
// groups take -32768 to 65535), a flag, a finite real, bytes or the text a binary group kept, or, for a text or
// handle group, the value as it is, to be spelled, so long as a real is finite. A failure carries no line; the
// writer knows it.
[[nodiscard]] DXFIO_API auto Typed(int code, const Content &held) -> std::expected<Content, Failure>;
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_TYPED_HPP */
