#include "checks.hpp"
#include "walk.hpp"

#include "dxfio/entities/record.hpp"
#include "dxfio/entity.hpp"
#include "dxfio/tables/layer.hpp"

#include <array>
#include <cstdlib>
#include <format>
#include <variant>

namespace DxfIO::Audit
{
  namespace
  {
    /**
     *  The lineweights AutoCAD has: ByLayer, ByBlock and the default, then its widths in hundredths
     *  of a millimeter.
     */
    constexpr std::array<int, 27> Known{-3, -2, -1,  0,   5,   9,   13,  15,  18,
                                        20, 25, 30,  35,  40,  50,  53,  60,  70,
                                        80, 90, 100, 106, 120, 140, 158, 200, 211};

    /**
     *  The lineweight AutoCAD has nearest to one, the lighter of two as near.
     */
    auto Nearest(int weight) -> int
    {
      int out = Known.front();
      for (const int one : Known)
      {
        if (std::abs(one - weight) < std::abs(out - weight))
        {
          out = one;
        }
      }
      return out;
    }

    template <class DocumentT>
    void Weighed(Auditor<DocumentT> &with, Part<DocumentT, int> &weight, Handle self)
    {
      const int nearest = Nearest(weight);
      if (nearest == weight)
      {
        return;
      }
      with.Note(Severity::Warning,
                std::format("lineweight {} is none AutoCAD has, {} taken", weight, nearest), self);
      if constexpr (Auditor<DocumentT>::Fix)
      {
        weight = nearest;
      }
    }

  } // namespace

  /**
   *  AutoCAD has 24 lineweights and three names; one of any other value is a warning, and takes the
   *  nearest when the checks fix, on a layer, an entity, or what an entity carries.
   */
  template <class DocumentT>
  void Weighted(Auditor<DocumentT> &with)
  {
    for (auto &layer : with.Drawing->Layer.Records)
    {
      Weighed(with, layer.Lineweight, layer.Record.Base.Self);
    }
    Lists(*with.Drawing,
          [&with](auto &all) -> void
          {
            for (auto &one : all)
            {
              std::visit(
                  [&with](auto &kind) -> void
                  {
                    if constexpr (requires { kind.Record.Lineweight; })
                    {
                      Weighed(with, kind.Record.Lineweight, kind.Record.Base.Self);
                    }
                    if constexpr (requires { kind.Vertices.Children; })
                    {
                      for (auto &child : kind.Vertices.Children)
                      {
                        Weighed(with, child.Record.Lineweight, child.Record.Base.Self);
                      }
                    }
                    if constexpr (requires { kind.Attributes.Children; })
                    {
                      for (auto &child : kind.Attributes.Children)
                      {
                        Weighed(with, child.Record.Lineweight, child.Record.Base.Self);
                      }
                    }
                  },
                  one);
            }
          });
  }

  template void Weighted(Auditor<Document> &with);

  template void Weighted(Auditor<const Document> &with);
} // namespace DxfIO::Audit
