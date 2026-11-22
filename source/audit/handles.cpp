#include "../based.hpp"
#include "../objects/shapes/defaulted.hpp"
#include "ascii.hpp"
#include "checks.hpp"
#include "entities/io.hpp"
#include "header/io.hpp"
#include "walk.hpp"

#include "dxfio/header/header.hpp"
#include "dxfio/version.hpp"

#include <cstddef>
#include <format>
#include <set>
#include <span>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO::Audit
{
  namespace
  {
    constexpr std::string_view Recorded = "block record";

    constexpr std::string_view Model = "*Model_Space";

    constexpr std::string_view Paper = "*Paper_Space";

    template <class DrawingT>
    struct Holder
    {
      Part<DrawingT, DxfIO::Base> *Base{nullptr};
      std::string_view             Kind;
      /**
       *  Whether it is a table's record or what an entity carries, listed after its table or entity
       *  and the others it shares that with: an owner that names the handle its table or entity
       *  gives up then follows it to its new one.
       */
      bool Led{};
    };

    template <class DrawingT, class ChainT>
    void Chained(ChainT &chain, std::vector<Holder<DrawingT>> &out)
    {
      for (auto &child : chain.Children)
      {
        out.push_back(Holder<DrawingT>{.Base = &child.Record.Base, .Kind = "entity", .Led = true});
      }
      if (chain.Closing)
      {
        out.push_back(Holder<DrawingT>{
            .Base = &(**chain.Closing).Record.Base, .Kind = "entity", .Led = true});
      }
    }

    template <class DrawingT>
    void Children(Part<DrawingT, Entity> &one, std::vector<Holder<DrawingT>> &out)
    {
      std::visit(
          [&out]<class KindT>(KindT &kind) -> void
          {
            if constexpr (requires { kind.Vertices.Children; })
            {
              Chained(kind.Vertices, out);
            }
            if constexpr (requires { kind.Attributes.Children; })
            {
              Chained(kind.Attributes, out);
            }
          },
          one);
    }

    template <class DrawingT, class TableT>
    void Listed(TableT &table, std::vector<Holder<DrawingT>> &out, std::string_view kind)
    {
      out.push_back(Holder<DrawingT>{.Base = &table.Control.Base, .Kind = "table"});
      for (auto &one : table.Records)
      {
        out.push_back(Holder<DrawingT>{.Base = &Based(one), .Kind = kind, .Led = true});
      }
    }

    template <class DrawingT>
    auto Holders(DrawingT &drawing) -> std::vector<Holder<DrawingT>>
    {
      using Kept = Part<DrawingT, DxfIO::Base>;
      std::vector<Holder<DrawingT>> out;
      out.reserve(drawing.Objects.size() + drawing.Entities.size());
      for (auto &one : drawing.Objects)
      {
        out.push_back(Holder<DrawingT>{
            .Base = &std::visit([](auto &kind) -> Kept & { return Based(kind); }, one),
            .Kind = "object",
        });
      }
      Listed(drawing.VPort, out, "record");
      Listed(drawing.LType, out, "record");
      Listed(drawing.Layer, out, "record");
      Listed(drawing.Style, out, "record");
      Listed(drawing.View, out, "record");
      Listed(drawing.Ucs, out, "record");
      Listed(drawing.AppId, out, "record");
      Listed(drawing.DimStyle, out, "record");
      Listed(drawing.BlockRecord, out, Recorded);
      for (auto &one : drawing.Loose)
      {
        Listed(one, out, "record");
      }
      for (auto &one : drawing.Blocks)
      {
        out.push_back(Holder<DrawingT>{.Base = &one.Block.Record.Base, .Kind = "block"});
        out.push_back(Holder<DrawingT>{.Base = &one.EndBlk.Record.Base, .Kind = "block end"});
      }
      Lists(drawing,
            [&out](auto &all) -> void
            {
              for (auto &one : all)
              {
                out.push_back(Holder<DrawingT>{
                    .Base = &std::visit([](auto &kind) -> Kept & { return Based(kind); }, one),
                    .Kind = Entities::Named(one),
                });
                Children<DrawingT>(one, out);
              }
            });
      return out;
    }

    /**
     *  Points what followed a block record's old handle to its new one: its block's BLOCK, ENDBLK
     *  and entities, those of the ENTITIES section for a model or paper space record, and its
     *  layout's pointer back to it.
     */
    void Blocked(Drawing &drawing, const Tables::BlockRecord &record, Handle old)
    {
      const Handle now    = record.Record.Base.Self;
      const auto   follow = [old, now](Handle &owner) -> void
      {
        if (owner == old)
        {
          owner = now;
        }
      };
      const std::string_view name = record.Record.Name;
      for (Blocks::Definition &one : drawing.Blocks)
      {
        if (!Ascii::Same(one.Block.Name, name))
        {
          continue;
        }
        follow(one.Block.Record.Base.Owner);
        follow(one.EndBlk.Record.Base.Owner);
        for (Entity &held : one.Entities)
        {
          follow(BaseOf(held).Owner);
        }
      }
      const bool paper = Ascii::Same(name, Paper);
      if (paper || Ascii::Same(name, Model))
      {
        for (Entity &one : drawing.Entities)
        {
          if (Entities::Paper(one) == paper)
          {
            follow(BaseOf(one).Owner);
          }
        }
      }
      for (Object &one : drawing.Objects)
      {
        auto *layout = std::get_if<Objects::Layout>(&one);
        if (layout != nullptr && layout->Base.Self == record.Layout)
        {
          follow(layout->Block);
        }
      }
    }

    /**
     *  Gives the first holder a new handle, and points the owners that followed its old one to it:
     *  of the holders it leads, listed right after it, and, for a block record, of its block.
     */
    void Retake(Auditor<Drawing> &with, std::span<const Holder<Drawing>> from)
    {
      const Holder<Drawing> &one = from.front();
      const Handle           old = one.Base->Self;
      one.Base->Self             = with.Take();
      for (std::size_t at = 1; !one.Led && at < from.size() && from[at].Led; ++at)
      {
        if (from[at].Base->Owner == old)
        {
          from[at].Base->Owner = one.Base->Self;
        }
      }
      if (one.Kind != Recorded)
      {
        return;
      }
      for (const Tables::BlockRecord &record : with.Drawing->BlockRecord.Records)
      {
        if (&record.Record.Base == one.Base)
        {
          Blocked(*with.Drawing, record, old);
        }
      }
    }
  } // namespace

  template <class DrawingT>
  void Handled(Auditor<DrawingT> &with)
  {
    if (Header::Declared(with.Drawing->Header) < Version::R13)
    {
      return;
    }
    const std::vector<Holder<DrawingT>> all = Holders(*with.Drawing);
    std::set<Handle>                    seen;
    for (std::size_t at = 0; at < all.size(); ++at)
    {
      const Holder<DrawingT> &one = all[at];
      if (!one.Base->Self)
      {
        with.Note(Severity::Error, std::format("a {} has no handle", one.Kind), {});
        if constexpr (Auditor<DrawingT>::Fix)
        {
          Retake(with, std::span{all}.subspan(at));
        }
      }
      else if (seen.contains(one.Base->Self))
      {
        with.Note(Severity::Error,
                  std::format("handle {:X} of a {} used twice", one.Base->Self.Value, one.Kind),
                  one.Base->Self);
        if constexpr (Auditor<DrawingT>::Fix)
        {
          Retake(with, std::span{all}.subspan(at));
        }
      }
      seen.insert(one.Base->Self);
    }
  }

  template void Handled(Auditor<Drawing> &with);

  template void Handled(Auditor<const Drawing> &with);
} // namespace DxfIO::Audit
