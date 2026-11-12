#ifndef DXFIO_ENTITIES_IO_HPP
#define DXFIO_ENTITIES_IO_HPP

#include "fields/context.hpp"
#include "file/record.hpp"
#include "seed.hpp"

#include "dxfio/api.hpp"
#include "dxfio/entities/polyline.hpp"
#include "dxfio/entities/vertex.hpp"
#include "dxfio/entity.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <set>
#include <span>
#include <string_view>
#include <vector>

namespace DxfIO::Entities
{
  [[nodiscard]] DXFIO_API auto Read(std::span<const File::Record> all, const Fields::Reading &with)
      -> List;
  [[nodiscard]] DXFIO_API auto Based(const Entity &one) -> const DxfIO::Base &;
  [[nodiscard]] DXFIO_API auto Named(const Entity &one) -> std::string_view;
  [[nodiscard]] DXFIO_API auto Paper(const Entity &one) -> bool;
  [[nodiscard]] DXFIO_API auto Top(const Entity &one) -> Handle;
  DXFIO_API void               Claim(Entity &one, Handle owner);
  /**
   *  The flags a vertex of that polyline carries: its own fitting bits, and the bit its polyline's
   *  kind asks, 32 of a 3D polyline, 64 of a polygon mesh, 128 for a face of a polyface mesh and 64
   *  with it for a vertex of one.
   */
  [[nodiscard]] DXFIO_API auto Asked(const Polyline &parent, const Vertex &one) -> int;
  /**
   *  Whether an entity is an ATTDEF that is not constant: one that makes its block an attributed
   *  block, flag 2.
   */
  [[nodiscard]] DXFIO_API auto Attributing(const Entity &one) -> bool;
  DXFIO_API void               Own(Entity &one, Seed &handles, Handle owner);
  DXFIO_API void               Bare(Entity &one, bool extended = true);
  DXFIO_API void               Loose(Entity &one, const std::set<Handle> &gone);
  [[nodiscard]] DXFIO_API auto Write(Fields::Writing &to, std::span<const Entity> all)
      -> std::expected<void, Failure>;
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_IO_HPP */
