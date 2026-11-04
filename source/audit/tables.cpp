#include "ascii.hpp"
#include "checks.hpp"
#include "header/io.hpp"
#include "walk.hpp"

#include "dxfio/create.hpp"
#include "dxfio/header/header.hpp"
#include "dxfio/version.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <ranges>
#include <span>
#include <string_view>
#include <variant>

namespace DxfIO::Audit
{
namespace
{
constexpr std::array Kinds{
    std::string_view{"ByBlock"},
    std::string_view{"ByLayer"},
    std::string_view{"Continuous"},
};

template <class DocumentT, class RecordT>
void Required(Auditor<DocumentT> &with, const Tables::Table<RecordT> &fresh,
              Part<DocumentT, Tables::Table<RecordT>> &table)
{
  std::size_t at = 0;
  for (const RecordT &one : fresh.Records)
  {
    const std::size_t place = std::min(at++, table.Records.size());
    if (Holds(table, one.Record.Name))
    {
      continue;
    }
    with.Note(Severity::Error, std::format("{} missing", one.Record.Name), table.Head.Base.Self);
    if constexpr (Auditor<DocumentT>::Fix)
    {
      RecordT made = one;
      made.Record.Base = {};
      made.Record.Base.Owner = table.Head.Base.Self;
      if (table.Head.Base.Self)
      {
        made.Record.Base.Self = with.Take();
      }
      Localized(made, *with.Drawing);
      table.Records.insert(std::next(table.Records.begin(), static_cast<std::ptrdiff_t>(place)), std::move(made));
    }
  }
}

template <class KindT>
auto Found(const Document &drawing, std::string_view name) -> Handle
{
  for (const Object &one : drawing.Objects)
  {
    const auto *held = std::get_if<KindT>(&one);
    if (held != nullptr && DxfIO::Ascii::Same(held->Name, name))
    {
      return held->Base.Self;
    }
  }
  return {};
}

auto Plotted(const Document &drawing) -> Handle
{
  for (const Object &one : drawing.Objects)
  {
    if (const auto *held = std::get_if<Objects::DictionaryWithDefault>(&one))
    {
      return held->Fallback;
    }
  }
  return {};
}

template <class DocumentT>
void Unstyled(Auditor<DocumentT> &with)
{
  const Handle style = Plotted(*with.Drawing);
  if (!style)
  {
    return;
  }
  for (auto &one : with.Drawing->Layer.Records)
  {
    if (one.Style || one.Record.Name.empty())
    {
      continue;
    }
    with.Note(Severity::Error, std::format("layer {} has no plot style", one.Record.Name), one.Record.Base.Self);
    if constexpr (Auditor<DocumentT>::Fix)
    {
      one.Style = style;
    }
  }
}

template <class RecordT>
auto Only(Tables::Table<RecordT> table, std::span<const std::string_view> names) -> Tables::Table<RecordT>
{
  std::erase_if(table.Records,
                [names](const RecordT &one) -> bool
                {
                  return std::ranges::none_of(names, [&one](std::string_view name) -> bool
                                              { return DxfIO::Ascii::Same(one.Record.Name, name); });
                });
  return table;
}
} // namespace

void Localized(Tables::Layer &one, const Document &drawing)
{
  one.Material = Found<Objects::Material>(drawing, "Global");
  one.Style = Plotted(drawing);
  one.Spare = {};
}

void Localized(Tables::Dimstyle &one, const Document &drawing)
{
  one.Dimtxsty = std::ranges::min_element(drawing.Style.Records, {}, [](const Tables::Style &style) -> bool
                                          { return !DxfIO::Ascii::Same(style.Record.Name, "Standard"); })
                     ->Record.Base.Self;
  one.Dimldrblk = {};
  one.Dimblk = {};
  one.Dimblk1 = {};
  one.Dimblk2 = {};
  one.Dimltype = {};
  one.Dimltex1 = {};
  one.Dimltex2 = {};
}

void Localized(Tables::Vport &one, const Document & /*drawing*/)
{
  one.Style = {};
  one.Background = {};
  one.Shadow = {};
}

template <class DocumentT>
void Tabled(Auditor<DocumentT> &with)
{
  DocumentT &drawing = *with.Drawing;
  const Document fresh = Create(Header::Declared(drawing.Header));
  const std::array zero{std::string_view{"0"}};
  const std::array standard{std::string_view{"Standard"}};
  const std::array acad{std::string_view{"ACAD"}};
  const std::array active{std::string_view{"*Active"}};
  Required(with, Only(fresh.Layer, zero), drawing.Layer);
  Required(with, Only(fresh.Ltype, Kinds), drawing.Ltype);
  Required(with, Only(fresh.Style, standard), drawing.Style);
  Required(with, Only(fresh.Dimstyle, standard), drawing.Dimstyle);
  Required(with, Only(fresh.Appid, acad), drawing.Appid);
  Required(with, Only(fresh.Vport, active), drawing.Vport);
  Unstyled(with);
}

template void Tabled(Auditor<Document> &with);

template void Tabled(Auditor<const Document> &with);
} // namespace DxfIO::Audit
