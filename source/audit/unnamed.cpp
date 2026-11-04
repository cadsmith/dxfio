#include "../masked.hpp"
#include "checks.hpp"

#include "dxfio/blocks/blocks.hpp"
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

// A STYLE that is a shape file has no name, as AutoCAD writes it.
auto Nameless(const Tables::Style &one) -> bool
{
  return one.Record.Name.empty() && Masked(one.Record.Flags, Shape) == 0;
}

// A record with no name is an error, and left out when the checks fix: AutoCAD refuses the file for it.
template <class DocumentT, class TableT>
void Listed(Auditor<DocumentT> &with, TableT &table, std::string_view kind)
{
  for (const auto &one : table.Records)
  {
    if (Nameless(one))
    {
      with.Note(Severity::Error, std::format("{} with no name, left out", kind), one.Record.Base.Self);
    }
  }
  if constexpr (Auditor<DocumentT>::Fix)
  {
    std::erase_if(table.Records, [](const auto &one) -> bool { return Nameless(one); });
  }
}

auto Blank(const Blocks::Block &one) -> bool
{
  return one.Begin.Name.empty();
}
} // namespace

template <class DocumentT>
void Unnamed(Auditor<DocumentT> &with)
{
  DocumentT &drawing = *with.Drawing;
  Listed(with, drawing.Vport, "viewport");
  Listed(with, drawing.Ltype, "linetype");
  Listed(with, drawing.Layer, "layer");
  Listed(with, drawing.Style, "text style");
  Listed(with, drawing.View, "view");
  Listed(with, drawing.Ucs, "ucs");
  Listed(with, drawing.Appid, "application");
  Listed(with, drawing.Dimstyle, "dimension style");
  Listed(with, drawing.BlockRecord, "block record");
  for (const Blocks::Block &one : drawing.Blocks)
  {
    if (Blank(one))
    {
      with.Note(Severity::Error, "block with no name, left out", one.Begin.Record.Base.Self);
    }
  }
  if constexpr (Auditor<DocumentT>::Fix)
  {
    std::erase_if(drawing.Blocks, Blank);
  }
}

template void Unnamed(Auditor<Document> &with);

template void Unnamed(Auditor<const Document> &with);
} // namespace DxfIO::Audit
