#include "../masked.hpp"
#include "checks.hpp"

#include "dxfio/blocks/definition.hpp"
#include "dxfio/tables/style.hpp"

#include <algorithm>
#include <format>
#include <string_view>

namespace DxfIO::Audit
{
  namespace
  {
    constexpr int Shape = 1;

    template <class RecordT>
    auto Nameless(const RecordT &one) -> bool
    {
      return one.Record.Name.empty();
    }

    /**
     *  A STYLE that is a shape file has no name, as AutoCAD writes it.
     */
    auto Nameless(const Tables::Style &one) -> bool
    {
      return one.Record.Name.empty() && Masked(one.Record.Flags, Shape) == 0;
    }

    /**
     *  A record with no name is an error, and left out when the checks fix: AutoCAD refuses the
     *  file for it.
     */
    template <class DrawingT, class TableT>
    void Listed(Auditor<DrawingT> &with, TableT &table, std::string_view kind)
    {
      for (const auto &one : table.Records)
      {
        if (Nameless(one))
        {
          with.Note(Severity::Error, std::format("{} with no name, left out", kind),
                    one.Record.Base.Self);
        }
      }
      if constexpr (Auditor<DrawingT>::Fix)
      {
        std::erase_if(table.Records, [](const auto &one) -> bool { return Nameless(one); });
      }
    }

    auto Blank(const Blocks::Definition &one) -> bool
    {
      return one.Block.Name.empty();
    }
  } // namespace

  template <class DrawingT>
  void Unnamed(Auditor<DrawingT> &with)
  {
    DrawingT &drawing = *with.Drawing;
    Listed(with, drawing.VPort, "viewport");
    Listed(with, drawing.LType, "linetype");
    Listed(with, drawing.Layer, "layer");
    Listed(with, drawing.Style, "text style");
    Listed(with, drawing.View, "view");
    Listed(with, drawing.Ucs, "ucs");
    Listed(with, drawing.AppId, "application");
    Listed(with, drawing.DimStyle, "dimension style");
    Listed(with, drawing.BlockRecord, "block record");
    for (const Blocks::Definition &one : drawing.Blocks)
    {
      if (Blank(one))
      {
        with.Note(Severity::Error, "block with no name, left out", one.Block.Record.Base.Self);
      }
    }
    if constexpr (Auditor<DrawingT>::Fix)
    {
      std::erase_if(drawing.Blocks, Blank);
    }
  }

  template void Unnamed(Auditor<Drawing> &with);

  template void Unnamed(Auditor<const Drawing> &with);
} // namespace DxfIO::Audit
