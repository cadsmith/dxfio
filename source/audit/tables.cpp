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

    template <class DrawingT, class RecordT>
    void Required(Auditor<DrawingT> &with, const Tables::Table<RecordT> &fresh,
                  Part<DrawingT, Tables::Table<RecordT>> &table)
    {
      std::size_t at = 0;
      for (const RecordT &one : fresh.Records)
      {
        const std::size_t place = std::min(at++, table.Records.size());
        if (Holds(table, one.Record.Name))
        {
          continue;
        }
        with.Note(Severity::Error, std::format("{} missing", one.Record.Name),
                  table.Control.Base.Self);
        if constexpr (Auditor<DrawingT>::Fix)
        {
          RecordT made           = one;
          made.Record.Base       = {};
          made.Record.Base.Owner = table.Control.Base.Self;
          if (table.Control.Base.Self)
          {
            made.Record.Base.Self = with.Take();
          }
          Localized(made, *with.Drawing);
          table.Records.insert(std::next(table.Records.begin(), static_cast<std::ptrdiff_t>(place)),
                               std::move(made));
        }
      }
    }

    template <class KindT>
    auto Found(const Drawing &drawing, std::string_view name) -> Handle
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

    auto Plotted(const Drawing &drawing) -> Handle
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

    template <class DrawingT>
    void Unstyled(Auditor<DrawingT> &with)
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
        with.Note(Severity::Error, std::format("layer {} has no plot style", one.Record.Name),
                  one.Record.Base.Self);
        if constexpr (Auditor<DrawingT>::Fix)
        {
          one.Style = style;
        }
      }
    }

    template <class RecordT>
    auto Only(Tables::Table<RecordT> table, std::span<const std::string_view> names)
        -> Tables::Table<RecordT>
    {
      std::erase_if(table.Records,
                    [names](const RecordT &one) -> bool
                    {
                      return std::ranges::none_of(
                          names, [&one](std::string_view name) -> bool
                          { return DxfIO::Ascii::Same(one.Record.Name, name); });
                    });
      return table;
    }
  } // namespace

  void Localized(Tables::Layer &one, const Drawing &drawing)
  {
    one.Material = Found<Objects::Material>(drawing, "Global");
    one.Style    = Plotted(drawing);
    one.Unknown1 = {};
  }

  void Localized(Tables::DimStyle &one, const Drawing &drawing)
  {
    one.Dimtxsty =
        std::ranges::min_element(drawing.Style.Records, {}, [](const Tables::Style &style) -> bool
                                 { return !DxfIO::Ascii::Same(style.Record.Name, "Standard"); })
            ->Record.Base.Self;
    one.Dimldrblk = {};
    one.Dimblk    = {};
    one.Dimblk1   = {};
    one.Dimblk2   = {};
    one.Dimltype  = {};
    one.Dimltex1  = {};
    one.Dimltex2  = {};
  }

  void Localized(Tables::VPort &one, const Drawing & /*drawing*/)
  {
    one.VisualStyle     = {};
    one.Background      = {};
    one.ShadePlotObject = {};
  }

  template <class DrawingT>
  void Tabled(Auditor<DrawingT> &with)
  {
    DrawingT        &drawing = *with.Drawing;
    const Drawing    fresh   = Create(Header::Declared(drawing.Header));
    const std::array zero{std::string_view{"0"}};
    const std::array standard{std::string_view{"Standard"}};
    const std::array acad{std::string_view{"ACAD"}};
    const std::array active{std::string_view{"*Active"}};
    Required(with, Only(fresh.Layer, zero), drawing.Layer);
    Required(with, Only(fresh.LType, Kinds), drawing.LType);
    Required(with, Only(fresh.Style, standard), drawing.Style);
    Required(with, Only(fresh.DimStyle, standard), drawing.DimStyle);
    Required(with, Only(fresh.AppId, acad), drawing.AppId);
    Required(with, Only(fresh.VPort, active), drawing.VPort);
    Unstyled(with);
  }

  template void Tabled(Auditor<Drawing> &with);

  template void Tabled(Auditor<const Drawing> &with);
} // namespace DxfIO::Audit
