#ifndef DXFIO_EDIT_FIND_HPP
#define DXFIO_EDIT_FIND_HPP

#include "dxfio/api.hpp"
#include "dxfio/blocks/blocks.hpp"
#include "dxfio/document.hpp"
#include "dxfio/entity.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/object.hpp"
#include "dxfio/tables/table.hpp"

#include <algorithm>
#include <string_view>

namespace DxfIO::Edit
{
  /**
   *  Tells whether two DXF names are the same, as AutoCAD compares them: without case.
   */
  [[nodiscard]] DXFIO_API auto Same(std::string_view one, std::string_view other) -> bool;

  /**
   *  Finds the entity of that handle in the ENTITIES section or in a block, or null. It looks at
   *  the entities of those lists only: the handle of a VERTEX, ATTRIB or SEQEND, which a POLYLINE
   *  or an INSERT carries in its Vertices or Attributes, finds null, and so does the null handle.
   *  The pointer holds until the list it points into changes, as Edit::Add and Audit::Repair change
   *  it.
   */
  [[nodiscard]] DXFIO_API auto FindEntity(Document &drawing, Handle self) -> Entity *;

  /**
   *  Finds the entity of that handle in the ENTITIES section or in a block, or null, as the other
   *  FindEntity does.
   */
  [[nodiscard]] DXFIO_API auto FindEntity(const Document &drawing, Handle self) -> const Entity *;

  /**
   *  Finds the object of that handle in the OBJECTS section, or null; the null handle finds null.
   */
  [[nodiscard]] DXFIO_API auto FindObject(Document &drawing, Handle self) -> Object *;

  /**
   *  Finds the object of that handle in the OBJECTS section, or null; the null handle finds null.
   */
  [[nodiscard]] DXFIO_API auto FindObject(const Document &drawing, Handle self) -> const Object *;

  /**
   *  Finds the block of that name, or null. The pointer holds until the list of blocks changes, as
   *  Edit::AddBlock changes it.
   */
  [[nodiscard]] DXFIO_API auto FindBlock(Document &drawing, std::string_view name)
      -> Blocks::Block *;

  /**
   *  Finds the block of that name, or null, as the other FindBlock does.
   */
  [[nodiscard]] DXFIO_API auto FindBlock(const Document &drawing, std::string_view name)
      -> const Blocks::Block *;

  /**
   *  Finds the record of that name in a table, such as `Find(drawing.Layer, "Walls")`, or null. The
   *  pointer holds until the table's records change, as Edit::Add, Edit::AddBlock and Audit::Repair
   *  change them.
   */
  template <class RecordT>
    requires requires(const RecordT &record) { record.Record.Name; }
  [[nodiscard]] auto Find(Tables::Table<RecordT> &table, std::string_view name) -> RecordT *
  {
    const auto found = std::ranges::find_if(table.Records, [name](const RecordT &record) -> bool
                                            { return Same(record.Record.Name, name); });
    return found == table.Records.end() ? nullptr : &*found;
  }

  /**
   *  Finds the record of that name in a table, or null, as the other Find does.
   */
  template <class RecordT>
    requires requires(const RecordT &record) { record.Record.Name; }
  [[nodiscard]] auto Find(const Tables::Table<RecordT> &table, std::string_view name)
      -> const RecordT *
  {
    const auto found = std::ranges::find_if(table.Records, [name](const RecordT &record) -> bool
                                            { return Same(record.Record.Name, name); });
    return found == table.Records.end() ? nullptr : &*found;
  }
} // namespace DxfIO::Edit

#endif /* DXFIO_EDIT_FIND_HPP */
