#include "../based.hpp"
#include "ascii.hpp"
#include "checks.hpp"
#include "names.hpp"
#include "walk.hpp"

#include <algorithm>
#include <format>
#include <string_view>
#include <variant>

namespace DxfIO::Audit
{
namespace
{
constexpr std::string_view Bylayer = "ByLayer";

constexpr std::string_view Byblock = "ByBlock";

constexpr std::string_view Standard = "Standard";

// The names the references are looked up in, gathered once for the whole check, and the audit they report to.
template <class DocumentT>
struct Known
{
  Auditor<DocumentT> *With{};
  Names Layers;
  Names Linetypes;
  Names Styles;
  Names Blocks;
};

template <class DocumentT>
void Layered(Known<DocumentT> &known, const Entities::Record &one)
{
  if (known.Layers.contains(one.Layer.View()))
  {
    return;
  }
  Auditor<DocumentT> &with = *known.With;
  with.Note(Severity::Error, std::format("layer {} missing", one.Layer.View()), one.Base.Self);
  if constexpr (Auditor<DocumentT>::Fix)
  {
    Tables::Table<Tables::Layer> &layers = with.Drawing->Layer;
    Tables::Layer made = *std::ranges::min_element(layers.Records, {}, [](const Tables::Layer &layer) -> bool
                                                   { return layer.Record.Name != "0"; });
    made.Record.Base = {};
    made.Record.Base.Owner = layers.Head.Base.Self;
    if (layers.Head.Base.Self)
    {
      made.Record.Base.Self = with.Take();
    }
    made.Record.Name = one.Layer.Str();
    known.Layers.emplace(made.Record.Name);
    layers.Records.push_back(std::move(made));
  }
}

template <class DocumentT, class RecordT>
void Lined(Known<DocumentT> &known, RecordT &one)
{
  const std::string_view name = one.Linetype.View();
  if (DxfIO::Ascii::Same(name, Bylayer) || DxfIO::Ascii::Same(name, Byblock) || known.Linetypes.contains(name))
  {
    return;
  }
  known.With->Note(Severity::Warning, std::format("linetype {} missing", name), one.Base.Self);
  if constexpr (Auditor<DocumentT>::Fix)
  {
    one.Linetype = Symbol{Bylayer};
  }
}

template <class DocumentT, class SymbolT>
void Styled(Known<DocumentT> &known, SymbolT &style, Handle self)
{
  if (known.Styles.contains(style.View()))
  {
    return;
  }
  known.With->Note(Severity::Warning, std::format("text style {} missing", style.View()), self);
  if constexpr (Auditor<DocumentT>::Fix)
  {
    style = Symbol{Standard};
  }
}

template <class DocumentT, class RecordT>
void Recorded(Known<DocumentT> &known, RecordT &one)
{
  Layered(known, one);
  Lined(known, one);
}

template <class DocumentT>
auto Orphan(Known<DocumentT> &known, const Entity &one) -> bool
{
  const auto *insert = std::get_if<Entities::Insert>(&one);
  if (insert == nullptr || known.Blocks.contains(insert->Block.View()))
  {
    return false;
  }
  known.With->Note(Severity::Error, std::format("block {} missing, its insert left out", insert->Block.View()),
                   insert->Record.Base.Self);
  return true;
}

template <class DocumentT, class KindT>
void Looked(Known<DocumentT> &known, KindT &kind);

template <class DocumentT, class ChainT>
void Carried(Known<DocumentT> &known, ChainT &chain)
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

template <class DocumentT, class KindT>
void Looked(Known<DocumentT> &known, KindT &kind)
{
  if constexpr (requires { kind.Record; })
  {
    Recorded(known, kind.Record);
  }
  if constexpr (requires { kind.Lettering.Style; })
  {
    Styled(known, kind.Lettering.Style, kind.Record.Base.Self);
  }
  if constexpr (requires { kind.Paragraph->Style; })
  {
    Styled(known, kind.Paragraph->Style, kind.Record.Base.Self);
  }
  if constexpr (requires { kind.Lettering->Style; })
  {
    Styled(known, kind.Lettering->Style, kind.Record.Base.Self);
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

template <class DocumentT, class EntityT>
void Checked(Known<DocumentT> &known, EntityT &one)
{
  std::visit([&known](auto &kind) -> void { Looked(known, kind); }, one);
}
} // namespace

template <class DocumentT>
void Referred(Auditor<DocumentT> &with)
{
  const DocumentT &drawing = *with.Drawing;
  Known<DocumentT> known{
      .With = &with,
      .Layers = Named(drawing.Layer),
      .Linetypes = Named(drawing.Ltype),
      .Styles = Named(drawing.Style),
      .Blocks = Defined(drawing.Blocks),
  };
  Lists(*with.Drawing,
        [&known](auto &all) -> void
        {
          for (auto &one : all)
          {
            Checked(known, one);
          }
          if constexpr (Auditor<DocumentT>::Fix)
          {
            std::erase_if(all, [&known](const Entity &one) -> bool { return Orphan(known, one); });
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

template void Referred(Auditor<Document> &with);

template void Referred(Auditor<const Document> &with);
} // namespace DxfIO::Audit
