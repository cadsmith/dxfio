#ifndef DXFIO_EDIT_INDEX_HPP
#define DXFIO_EDIT_INDEX_HPP

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/entity.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/object.hpp"

#include <cstdint>
#include <unordered_map>

namespace DxfIO::Edit
{
  /**
   *  A document's entities and objects by handle, read once so that each lookup takes constant
   *  time: what finding many handles needs, where FindEntity and FindObject walk the lists each
   *  time.
   *
   *  It finds what they find, the first in their order when several share a handle, and holds
   *  pointers into the document's lists, good until a list changes; the document must outlive it.
   */
  class DXFIO_API Index
  {
  public:
    explicit Index(const Document &drawing);

    [[nodiscard]] auto Drawn() const -> const Document &;

    /**
     *  Finds the entity of that handle in the ENTITIES section or in a block, or null, as
     *  FindEntity does.
     */
    [[nodiscard]] auto FindEntity(Handle self) const -> const Entity *;

    /**
     *  Finds the object of that handle in the OBJECTS section, or null, as FindObject does.
     */
    [[nodiscard]] auto FindObject(Handle self) const -> const Object *;

  private:
    const Document                                   *m_Drawing;
    std::unordered_map<std::uint64_t, const Entity *> m_Entities;
    std::unordered_map<std::uint64_t, const Object *> m_Objects;
  };
} // namespace DxfIO::Edit

#endif /* DXFIO_EDIT_INDEX_HPP */
