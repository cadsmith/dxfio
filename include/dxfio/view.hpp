#ifndef DXFIO_VIEW_HPP
#define DXFIO_VIEW_HPP

#include "dxfio/api.hpp"
#include "dxfio/blocks/definition.hpp"
#include "dxfio/drawing.hpp"
#include "dxfio/object.hpp"
#include "dxfio/objects/dictionary.hpp"
#include "dxfio/objects/layout.hpp"
#include "dxfio/tables/block.hpp"
#include "dxfio/tables/table.hpp"

#include <algorithm>
#include <concepts>
#include <string_view>
#include <type_traits>
#include <vector>

/**
 *  A drawing read by its structure: the records of its tables and its blocks by name, its layouts
 *  in tab order, its dictionaries by name.
 *
 *  Every function gives pointers into the drawing, good until the list they point into changes, as
 *  Edit::Add, Edit::AddBlock and Audit::Repair change them, or null.
 */
namespace DxfIO::View
{
  /**
   *  Tells whether two DXF names are the same, as AutoCAD compares them: without case.
   */
  [[nodiscard]] DXFIO_API auto Same(std::string_view one, std::string_view other) -> bool;

  [[nodiscard]] DXFIO_API auto Block(Drawing &drawing, std::string_view name)
      -> Blocks::Definition *;

  [[nodiscard]] DXFIO_API auto Block(const Drawing &drawing, std::string_view name)
      -> const Blocks::Definition *;

  /**
   *  The record of that name in a table, such as `Record(drawing.Layer, "Walls")`; one of the
   *  tables the drawing keeps as Loose too.
   */
  template <class RecordT>
    requires requires(const RecordT &record) { record.Record.Name; }
  [[nodiscard]] auto Record(Tables::Table<RecordT> &table, std::string_view name) -> RecordT *
  {
    const auto found = std::ranges::find_if(table.Records, [name](const RecordT &record) -> bool
                                            { return Same(record.Record.Name, name); });
    return found == table.Records.end() ? nullptr : &*found;
  }

  template <class RecordT>
    requires requires(const RecordT &record) { record.Record.Name; }
  [[nodiscard]] auto Record(const Tables::Table<RecordT> &table, std::string_view name)
      -> const RecordT *
  {
    const auto found = std::ranges::find_if(table.Records, [name](const RecordT &record) -> bool
                                            { return Same(record.Record.Name, name); });
    return found == table.Records.end() ? nullptr : &*found;
  }

  /**
   *  The table of the drawing that holds records of that kind, such as `drawing.Layer` for
   *  Tables::Layer.
   */
  template <class RecordT, class DrawingT>
    requires std::same_as<std::remove_const_t<DrawingT>, Drawing>
  [[nodiscard]] auto TableOf(DrawingT &drawing) -> auto &
  {
    if constexpr (std::same_as<RecordT, Tables::VPort>)
    {
      return drawing.VPort;
    }
    else if constexpr (std::same_as<RecordT, Tables::LType>)
    {
      return drawing.LType;
    }
    else if constexpr (std::same_as<RecordT, Tables::Layer>)
    {
      return drawing.Layer;
    }
    else if constexpr (std::same_as<RecordT, Tables::Style>)
    {
      return drawing.Style;
    }
    else if constexpr (std::same_as<RecordT, Tables::View>)
    {
      return drawing.View;
    }
    else if constexpr (std::same_as<RecordT, Tables::Ucs>)
    {
      return drawing.Ucs;
    }
    else if constexpr (std::same_as<RecordT, Tables::AppId>)
    {
      return drawing.AppId;
    }
    else if constexpr (std::same_as<RecordT, Tables::DimStyle>)
    {
      return drawing.DimStyle;
    }
    else
    {
      static_assert(std::same_as<RecordT, Tables::BlockRecord>, "a kind of table record");
      return drawing.BlockRecord;
    }
  }

  /**
   *  The record of that name in the table of its kind, such as `Record<Tables::Layer>(drawing,
   *  "Walls")`.
   */
  template <class RecordT>
  [[nodiscard]] auto Record(Drawing &drawing, std::string_view name) -> RecordT *
  {
    return Record(TableOf<RecordT>(drawing), name);
  }

  template <class RecordT>
  [[nodiscard]] auto Record(const Drawing &drawing, std::string_view name) -> const RecordT *
  {
    return Record(TableOf<RecordT>(drawing), name);
  }

  /**
   *  The BLOCK_RECORD of *Model_Space; null before R13, which has no such table.
   */
  [[nodiscard]] DXFIO_API auto ModelSpace(const Drawing &drawing) -> const Tables::BlockRecord *;

  /**
   *  The BLOCK_RECORD of *Paper_Space, the active layout's paper space; null before R13.
   */
  [[nodiscard]] DXFIO_API auto PaperSpace(const Drawing &drawing) -> const Tables::BlockRecord *;

  /**
   *  In tab order, Model first.
   */
  [[nodiscard]] DXFIO_API auto Layouts(const Drawing &drawing)
      -> std::vector<const Objects::Layout *>;

  [[nodiscard]] DXFIO_API auto Layout(const Drawing &drawing, std::string_view name)
      -> const Objects::Layout *;

  /**
   *  The named objects dictionary: the first dictionary of the OBJECTS section that no other object
   *  owns.
   */
  [[nodiscard]] DXFIO_API auto Root(const Drawing &drawing) -> const Objects::Dictionary *;

  /**
   *  The object the dictionary's entry of that name leads to, whether it owns it hard or soft.
   */
  [[nodiscard]] DXFIO_API auto Entry(const Drawing &drawing, const Objects::Dictionary &dictionary,
                                     std::string_view name) -> const Object *;

  /**
   *  The dictionary the root's entry of that name leads to, such as ACAD_LAYOUT.
   */
  [[nodiscard]] DXFIO_API auto Dictionary(const Drawing &drawing, std::string_view name)
      -> const Objects::Dictionary *;
} // namespace DxfIO::View

#endif /* DXFIO_VIEW_HPP */
