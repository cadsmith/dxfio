#include "checks.hpp"
#include "entities/io.hpp"
#include "walk.hpp"

#include <format>
#include <string_view>
#include <utility>
#include <variant>

namespace DxfIO::Audit
{
namespace
{
template <class KindT>
auto Chain(KindT &kind) -> auto &
{
  if constexpr (requires { kind.Vertices.Children; })
  {
    return kind.Vertices;
  }
  else
  {
    return kind.Attributes;
  }
}

// Whether a SEQEND must follow it in the file: always for a POLYLINE, for an INSERT when attributes follow it.
template <class KindT>
auto Needed(const KindT &kind) -> bool
{
  if constexpr (requires { kind.Vertices.Children; })
  {
    return true;
  }
  else
  {
    return kind.Follows || !kind.Attributes.Children.empty();
  }
}

// An INSERT whose attributes are not flagged to follow it, group 66, has them read as entities on their own.
template <class DocumentT, class InsertT>
void Flagged(Auditor<DocumentT> &with, InsertT &insert)
{
  if (insert.Attributes.Children.empty() || insert.Follows)
  {
    return;
  }
  with.Note(Severity::Error, "INSERT attributes not marked to follow it", insert.Record.Base.Self);
  if constexpr (Auditor<DocumentT>::Fix)
  {
    insert.Follows = true;
  }
}

// The SEQEND that closes the vertices or attributes, when missing, is made on the entity's layer, owned by it.
template <class DocumentT, class KindT>
void Ended(Auditor<DocumentT> &with, KindT &kind, std::string_view name)
{
  auto &chain = Chain(kind);
  if (chain.Closing || !Needed(kind))
  {
    return;
  }
  with.Note(Severity::Error, std::format("{} has no SEQEND", name), kind.Record.Base.Self);
  if constexpr (Auditor<DocumentT>::Fix)
  {
    Entities::Record record = kind.Record;
    record.Base = {};
    if (kind.Record.Base.Self)
    {
      record.Base.Self = with.Take();
    }
    record.Base.Owner = kind.Record.Base.Self;
    chain.Closing = Boxed{Entities::Seqend{.Record = std::move(record)}};
  }
}

// A vertex flagged for another kind of polyline than its own is written under the wrong subclass, which AutoCAD
// refuses: an error, and the flags its polyline asks when the checks fix.
template <class DocumentT, class PolylineT>
void Reflagged(Auditor<DocumentT> &with, PolylineT &polyline)
{
  for (auto &vertex : polyline.Vertices.Children)
  {
    const int asked = Entities::Asked(polyline, vertex);
    if (vertex.Flags == asked)
    {
      continue;
    }
    with.Note(Severity::Error, std::format("vertex flagged {} where its polyline asks {}", vertex.Flags, asked),
              vertex.Record.Base.Self);
    if constexpr (Auditor<DocumentT>::Fix)
    {
      vertex.Flags = asked;
    }
  }
}

} // namespace

template <class DocumentT>
void Sequenced(Auditor<DocumentT> &with)
{
  Lists(*with.Drawing,
        [&with](auto &all) -> void
        {
          for (auto &one : all)
          {
            std::visit(
                [&with](auto &kind) -> void
                {
                  if constexpr (requires { kind.Attributes.Children; })
                  {
                    Flagged(with, kind);
                    Ended(with, kind, "INSERT");
                  }
                  if constexpr (requires { kind.Vertices.Children; })
                  {
                    Reflagged(with, kind);
                    Ended(with, kind, "POLYLINE");
                  }
                },
                one);
          }
        });
}

template void Sequenced(Auditor<Document> &with);

template void Sequenced(Auditor<const Document> &with);
} // namespace DxfIO::Audit
