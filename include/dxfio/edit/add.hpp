#ifndef DXFIO_EDIT_ADD_HPP
#define DXFIO_EDIT_ADD_HPP

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/edit/space.hpp"
#include "dxfio/entity.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/handle.hpp"

#include <expected>
#include <string_view>

namespace DxfIO::Edit
{
  /**
   *  Adds an entity to model or paper space and returns its handle. The entity, and the vertices or
   *  attributes it carries, take new handles where they have none, and are owned by the space's
   *  block record. A POLYLINE, and an INSERT with attributes, are given the SEQEND that closes them
   *  when they have none, on their own layer, and an INSERT with attributes says they follow it. A
   *  POLYLINE's vertices take the flags its kind asks: 32 of a 3D polyline, 64 of a polygon mesh,
   *  128 for a face. An ATTDEF that is not constant flags the block it goes to, the space's
   *  included, as holding attribute definitions.
   */
  DXFIO_API auto Add(Document &drawing, Entity one, Space space = Space::Model)
      -> std::expected<Handle, Failure>;

  /**
   *  Adds an entity to the block of that name, as the other Add does to a space, and returns its
   *  handle. Fails when there is no such block.
   */
  DXFIO_API auto Add(Document &drawing, Entity one, std::string_view block)
      -> std::expected<Handle, Failure>;

  /**
   *  Adds a block of that name, with its BLOCK_RECORD where the drawing has block records, and
   *  returns the handle of its BLOCK. Fails when the name is empty or the drawing has a block of
   *  that name.
   */
  DXFIO_API auto AddBlock(Document &drawing, std::string_view name)
      -> std::expected<Handle, Failure>;

  /**
   *  Adds a record to its table, owned by the table, and returns its handle. Fails when the record
   *  has no name, or when the table holds a record of that name, but for VPORT, whose tiled
   *  viewports share one.
   */
  DXFIO_API auto Add(Document &drawing, Tables::Vport one) -> std::expected<Handle, Failure>;

  /**
   *  Adds a linetype. Fails when the LTYPE table holds one of that name.
   */
  DXFIO_API auto Add(Document &drawing, Tables::Ltype one) -> std::expected<Handle, Failure>;

  /**
   *  Adds a layer. One that names no plot style or material takes layer 0's: AutoCAD refuses a
   *  layer with no plot style from R2000 on. Fails when the LAYER table holds one of that name.
   */
  DXFIO_API auto Add(Document &drawing, Tables::Layer one) -> std::expected<Handle, Failure>;

  /**
   *  Adds a text style. Fails when the STYLE table holds one of that name.
   */
  DXFIO_API auto Add(Document &drawing, Tables::Style one) -> std::expected<Handle, Failure>;

  /**
   *  Adds a named view. Fails when the VIEW table holds one of that name.
   */
  DXFIO_API auto Add(Document &drawing, Tables::View one) -> std::expected<Handle, Failure>;

  /**
   *  Adds a user coordinate system. Fails when the UCS table holds one of that name.
   */
  DXFIO_API auto Add(Document &drawing, Tables::Ucs one) -> std::expected<Handle, Failure>;

  /**
   *  Registers an application for extended data. Fails when the APPID table holds one of that name.
   */
  DXFIO_API auto Add(Document &drawing, Tables::Appid one) -> std::expected<Handle, Failure>;

  /**
   *  Adds a dimension style. Fails when the DIMSTYLE table holds one of that name.
   */
  DXFIO_API auto Add(Document &drawing, Tables::Dimstyle one) -> std::expected<Handle, Failure>;
} // namespace DxfIO::Edit

#endif /* DXFIO_EDIT_ADD_HPP */
