#ifndef DXFIO_DRAWING_HPP
#define DXFIO_DRAWING_HPP

#include "dxfio/api.hpp"
#include "dxfio/blocks/definition.hpp"
#include "dxfio/entity.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/header/header.hpp"
#include "dxfio/object.hpp"
#include "dxfio/raw.hpp"
#include "dxfio/section.hpp"
#include "dxfio/tables/appid.hpp"
#include "dxfio/tables/block.hpp"
#include "dxfio/tables/dimstyle.hpp"
#include "dxfio/tables/layer.hpp"
#include "dxfio/tables/ltype.hpp"
#include "dxfio/tables/style.hpp"
#include "dxfio/tables/table.hpp"
#include "dxfio/tables/ucs.hpp"
#include "dxfio/tables/view.hpp"
#include "dxfio/tables/vport.hpp"
#include "dxfio/unknown.hpp"

#include <string>
#include <vector>

namespace DxfIO
{
  /**
   *  A DXF drawing: one member for each section, and one for each table of the TABLES section. A
   *  table whose head has no name, as when the file had no such table, is not written.
   */
  struct DXFIO_API Drawing
  {
    /**
     *  The names of the sections, such as "HEADER", in the order the file gave them. Save writes
     *  these, in this order, and after them any other section the drawing holds something for, each
     *  where DXF puts it.
     */
    std::vector<std::string> Order{};
    /**
     *  The HEADER section: each $ variable with its groups, in the order the file gave them.
     */
    std::vector<Header::Variable> Header{};
    /**
     *  The CLASSES section, from R13: each CLASS record as its groups.
     */
    std::vector<Raw> Classes{};
    /**
     *  The VPORT table: the tiled viewport configurations of model space.
     */
    Tables::Table<Tables::VPort> VPort{};
    /**
     *  The LTYPE table: the linetypes.
     */
    Tables::Table<Tables::LType> LType{};
    /**
     *  The LAYER table.
     */
    Tables::Table<Tables::Layer> Layer{};
    /**
     *  The STYLE table: the text styles and the shape files.
     */
    Tables::Table<Tables::Style> Style{};
    /**
     *  The VIEW table: the named views.
     */
    Tables::Table<Tables::View> View{};
    /**
     *  The UCS table: the named user coordinate systems.
     */
    Tables::Table<Tables::Ucs> Ucs{};
    /**
     *  The APPID table: the applications registered to attach extended data.
     */
    Tables::Table<Tables::AppId> AppId{};
    /**
     *  The DIMSTYLE table: the dimension styles.
     */
    Tables::Table<Tables::DimStyle> DimStyle{};
    /**
     *  The BLOCK_RECORD table, from R13: one record for each block.
     */
    Tables::Table<Tables::BlockRecord> BlockRecord{};
    /**
     *  The other tables of the TABLES section, of names dxfio has no type for, their records kept
     *  as Unknown.
     */
    std::vector<Tables::Table<Unknown>> Loose{};
    /**
     *  The BLOCKS section: each block with its entities.
     */
    std::vector<Blocks::Definition> Blocks{};
    /**
     *  The ENTITIES section: the entities of model space and of the active paper space layout.
     */
    DxfIO::Entities::List Entities{};
    /**
     *  The OBJECTS section, from R13: the dictionaries, layouts and other objects that draw
     *  nothing.
     */
    std::vector<Object> Objects{};
    /**
     *  The sections dxfio has no type for, such as THUMBNAILIMAGE or ACDSDATA, each kept as its
     *  groups.
     */
    std::vector<Section> Kept{};

    auto operator==(const Drawing &) const -> bool = default;
  };

  /**
   *  Finds the entity of that handle in the ENTITIES section or in a block, or null. It looks at
   *  the entities of those lists only: the handle of a VERTEX, ATTRIB or SEQEND, which a POLYLINE
   *  or an INSERT carries in its Vertices or Attributes, finds null, and so does the null handle.
   *  The pointer holds until the list it points into changes, as Edit::Add and Audit::Repair change
   *  it.
   */
  [[nodiscard]] DXFIO_API auto FindEntity(Drawing &drawing, Handle self) -> Entity *;

  /**
   *  Finds the entity of that handle in the ENTITIES section or in a block, or null, as the other
   *  FindEntity does.
   */
  [[nodiscard]] DXFIO_API auto FindEntity(const Drawing &drawing, Handle self) -> const Entity *;

  /**
   *  Finds the object of that handle in the OBJECTS section, or null; the null handle finds null.
   */
  [[nodiscard]] DXFIO_API auto FindObject(Drawing &drawing, Handle self) -> Object *;

  /**
   *  Finds the object of that handle in the OBJECTS section, or null; the null handle finds null.
   */
  [[nodiscard]] DXFIO_API auto FindObject(const Drawing &drawing, Handle self) -> const Object *;
} // namespace DxfIO

#endif /* DXFIO_DRAWING_HPP */
