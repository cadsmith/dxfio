#include "../based.hpp"
#include "ascii.hpp"
#include "checks.hpp"
#include "names.hpp"
#include "walk.hpp"

#include "dxfio/entities/leader.hpp"
#include "dxfio/entities/mtext.hpp"
#include "dxfio/entities/tolerance.hpp"

#include <algorithm>
#include <format>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace DxfIO::Audit
{
  namespace
  {
    constexpr std::string_view Bylayer = "ByLayer";

    constexpr std::string_view Byblock = "ByBlock";

    constexpr std::string_view Standard = "Standard";

    /**
     *  The names the references are looked up in, gathered once for the whole check, and the audit
     *  they report to.
     */
    template <class DrawingT>
    struct Known
    {
      Auditor<DrawingT> *With{};
      Names              Layers;
      Names              Linetypes;
      Names              Styles;
      Names              Dimstyles;
      Names              Blocks;
    };

    /**
     *  What a missing name is called: the name, or none.
     */
    auto Absent(std::string_view kind, std::string_view name) -> std::string
    {
      return name.empty() ? std::format("{} with no name", kind)
                          : std::format("{} {} missing", kind, name);
    }

    /**
     *  An entity on a layer with no name goes to layer 0: a layer cannot be made for a name AutoCAD
     *  refuses.
     */
    template <class DrawingT>
    void Grounded(Known<DrawingT> &known, Part<DrawingT, Entities::Record> &one)
    {
      known.With->Note(Severity::Error, "layer with no name, 0 taken", one.Base.Self);
      if constexpr (Auditor<DrawingT>::Fix)
      {
        one.Layer = Symbol{"0"};
      }
    }

    template <class DrawingT>
    void Layered(Known<DrawingT> &known, Part<DrawingT, Entities::Record> &one)
    {
      if (one.Layer.View().empty())
      {
        Grounded(known, one);
        return;
      }
      if (known.Layers.contains(one.Layer.View()))
      {
        return;
      }
      Auditor<DrawingT> &with = *known.With;
      with.Note(Severity::Error, std::format("layer {} missing", one.Layer.View()), one.Base.Self);
      if constexpr (Auditor<DrawingT>::Fix)
      {
        Tables::Table<Tables::Layer> &layers = with.Drawing->Layer;
        Tables::Layer                 made =
            *std::ranges::min_element(layers.Records, {}, [](const Tables::Layer &layer) -> bool
                                      { return layer.Record.Name != "0"; });
        made.Record.Base       = {};
        made.Record.Base.Owner = layers.Control.Base.Self;
        if (layers.Control.Base.Self)
        {
          made.Record.Base.Self = with.Take();
        }
        made.Record.Name = one.Layer.Str();
        known.Layers.emplace(made.Record.Name);
        layers.Records.push_back(std::move(made));
      }
    }

    template <class DrawingT, class RecordT>
    void Lined(Known<DrawingT> &known, RecordT &one)
    {
      const std::string_view name = one.Linetype.View();
      if (DxfIO::Ascii::Same(name, Bylayer)
          || DxfIO::Ascii::Same(name, Byblock)
          || known.Linetypes.contains(name))
      {
        return;
      }
      known.With->Note(Severity::Warning, Absent("linetype", name), one.Base.Self);
      if constexpr (Auditor<DrawingT>::Fix)
      {
        one.Linetype = Symbol{Bylayer};
      }
    }

    template <class DrawingT, class SymbolT>
    void Styled(Known<DrawingT> &known, SymbolT &style, Handle self)
    {
      if (known.Styles.contains(style.View()))
      {
        return;
      }
      known.With->Note(Severity::Warning, Absent("text style", style.View()), self);
      if constexpr (Auditor<DrawingT>::Fix)
      {
        style = Symbol{Standard};
      }
    }

    template <class DrawingT, class SymbolT>
    void Dimstyled(Known<DrawingT> &known, SymbolT &style, Handle self)
    {
      if (known.Dimstyles.contains(style.View()))
      {
        return;
      }
      known.With->Note(Severity::Warning, Absent("dimension style", style.View()), self);
      if constexpr (Auditor<DrawingT>::Fix)
      {
        style = Symbol{Standard};
      }
    }

    template <class DrawingT, class RecordT>
    void Recorded(Known<DrawingT> &known, RecordT &one)
    {
      Layered(known, one);
      Lined(known, one);
    }

    template <class DrawingT>
    auto Orphan(Known<DrawingT> &known, const Entity &one) -> bool
    {
      const auto *insert = std::get_if<Entities::Insert>(&one);
      if (insert == nullptr || known.Blocks.contains(insert->Block.View()))
      {
        return false;
      }
      known.With->Note(
          Severity::Error,
          std::format("{}, its insert left out", Absent("block", insert->Block.View())),
          insert->Record.Base.Self);
      return true;
    }

    template <class DrawingT, class KindT>
    void Looked(Known<DrawingT> &known, KindT &kind);

    template <class DrawingT, class ChainT>
    void Carried(Known<DrawingT> &known, ChainT &chain)
    {
      for (auto &child : chain.Children)
      {
        Looked(known, child);
      }
      if (chain.Closing)
      {
        Looked(known, **chain.Closing);
      }
    }

    template <class DrawingT, class KindT>
    void Looked(Known<DrawingT> &known, KindT &kind)
    {
      if constexpr (requires { kind.Record; })
      {
        Recorded(known, kind.Record);
      }
      if constexpr (requires { kind.Lettering.Style; })
      {
        Styled(known, kind.Lettering.Style, kind.Record.Base.Self);
      }
      if constexpr (std::is_same_v<std::remove_cvref_t<KindT>, Entities::MText>)
      {
        Styled(known, kind.Data->Style, kind.Record.Base.Self);
      }
      if constexpr (requires { kind.Lettering->Style; })
      {
        Styled(known, kind.Lettering->Style, kind.Record.Base.Self);
      }
      if constexpr (requires { kind.Data->Measure.Style; })
      {
        Dimstyled(known, kind.Data->Measure.Style, kind.Record.Base.Self);
      }
      if constexpr (std::is_same_v<std::remove_cvref_t<KindT>, Entities::Leader>)
      {
        Dimstyled(known, kind.Data->Style, kind.Record.Base.Self);
      }
      if constexpr (std::is_same_v<std::remove_cvref_t<KindT>, Entities::Tolerance>)
      {
        Dimstyled(known, kind.Style, kind.Record.Base.Self);
      }
      if constexpr (requires { kind.Vertices.Children; })
      {
        Carried(known, kind.Vertices);
      }
      if constexpr (requires { kind.Attributes.Children; })
      {
        Carried(known, kind.Attributes);
      }
    }

    template <class DrawingT, class EntityT>
    void Checked(Known<DrawingT> &known, EntityT &one)
    {
      std::visit([&known](auto &kind) -> void { Looked(known, kind); }, one);
    }
  } // namespace

  template <class DrawingT>
  void Referred(Auditor<DrawingT> &with)
  {
    const DrawingT &drawing = *with.Drawing;
    Known<DrawingT> known{
        .With      = &with,
        .Layers    = Named(drawing.Layer),
        .Linetypes = Named(drawing.LType),
        .Styles    = Named(drawing.Style),
        .Dimstyles = Named(drawing.DimStyle),
        .Blocks    = Defined(drawing.Blocks),
    };
    Lists(*with.Drawing,
          [&known](auto &all) -> void
          {
            for (auto &one : all)
            {
              Checked(known, one);
            }
            if constexpr (Auditor<DrawingT>::Fix)
            {
              std::erase_if(all,
                            [&known](const Entity &one) -> bool { return Orphan(known, one); });
            }
            else
            {
              for (const Entity &one : all)
              {
                Orphan(known, one);
              }
            }
          });
  }

  template void Referred(Auditor<Drawing> &with);

  template void Referred(Auditor<const Drawing> &with);
} // namespace DxfIO::Audit
