#ifndef DXFIO_ENTITIES_SEQUENCE_HPP
#define DXFIO_ENTITIES_SEQUENCE_HPP

#include "dxfio/boxed.hpp"
#include "dxfio/entities/seqend.hpp"

#include <optional>
#include <vector>

namespace DxfIO::Entities
{
/// The entities that follow a POLYLINE or an INSERT in the file, up to the SEQEND that closes them.
template <class ChildT>
struct Sequence
{
  /// The vertices or attributes, in the order they come.
  std::vector<ChildT> Children;
  /// The SEQEND that closes them, or none when the file had none.
  std::optional<Boxed<Entities::Seqend>> Closing;

  auto operator==(const Sequence &) const -> bool = default;
};
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SEQUENCE_HPP */
