#ifndef DXFIO_ENTITIES_SEQEND_HPP
#define DXFIO_ENTITIES_SEQEND_HPP

#include "dxfio/api.hpp"
#include "dxfio/entities/record.hpp"

namespace DxfIO::Entities
{
/// A SEQEND: the entity that closes the vertices of a POLYLINE or the attributes of an INSERT.
struct DXFIO_API Seqend
{
  /// The layer, linetype, color and the rest every entity carries.
  Entities::Record Record{};

  auto operator==(const Seqend &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SEQEND_HPP */
