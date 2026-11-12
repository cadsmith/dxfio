#ifndef DXFIO_TABLES_BLOCK_HPP
#define DXFIO_TABLES_BLOCK_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"
#include "dxfio/tables/record.hpp"

#include <vector>

namespace DxfIO::Tables
{
  /**
   *  A BLOCK_RECORD table record: the owner of a block's entities, and the layout of a model or
   *  paper space block.
   */
  struct DXFIO_API BlockRecord
  {
    /**
     *  The name, flags and handles every table record carries.
     */
    Tables::Record Record{};
    /**
     *  The LAYOUT of a model or paper space block; none for any other block. Group 340, from R2000.
     */
    Handle Layout{};
    /**
     *  The INSERT entities of the block, as AutoCAD lists them in a 102 {BLKREFS brace after 340:
     *  group 331 each, from R2000. Empty writes no brace.
     */
    std::vector<Handle> Inserts{};
    /**
     *  The units the block is inserted in, as $INSUNITS gives them: 0 none, 1 inches, 4 millimeters
     *  and so on. Group 70, from R2007.
     */
    int Units{};
    /**
     *  Whether an insert of the block can be exploded. Group 280, from R2007.
     */
    bool Explodable{true};
    /**
     *  Whether an insert of the block scales the same along every axis. Group 281, from R2007.
     */
    bool Uniform{};

    auto operator==(const BlockRecord &) const -> bool = default;
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_BLOCK_HPP */
