#ifndef DXFIO_FIELDS_OWNED_HPP
#define DXFIO_FIELDS_OWNED_HPP

#include "encoding/encoding.hpp"
#include "fields/context.hpp"
#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/group.hpp"

#include <expected>
#include <span>
#include <string>

namespace DxfIO::Fields
{
  [[nodiscard]] DXFIO_API auto Owned(const Tags::Tag &one, const Encoding::Scheme &with) -> Group;

  [[nodiscard]] DXFIO_API auto Appended(std::string &out, const Tags::Tag &one) -> bool;

  [[nodiscard]] DXFIO_API auto Put(Writing &to, const Group &one) -> std::expected<void, Failure>;

  [[nodiscard]] DXFIO_API auto Put(Writing &to, std::span<const Group> all)
      -> std::expected<void, Failure>;
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_OWNED_HPP */
