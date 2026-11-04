#include "checks.hpp"
#include "walk.hpp"

#include <format>
#include <variant>

namespace DxfIO::Audit
{
template <class DocumentT>
void Meshed(Auditor<DocumentT> &with)
{
  Lists(*with.Drawing,
        [&with](auto &all) -> void
        {
          for (auto &one : all)
          {
            auto *mesh = std::get_if<Entities::Mesh>(&one);
            if (mesh == nullptr)
            {
              continue;
            }
            const std::size_t edges = mesh->Net->Edges.size() / 2;
            if (mesh->Net->Creases.size() == edges)
            {
              continue;
            }
            with.Note(Severity::Warning,
                      std::format("mesh with {} creases for {} edges", mesh->Net->Creases.size(), edges),
                      mesh->Record.Base.Self);
            if constexpr (Auditor<DocumentT>::Fix)
            {
              mesh->Net->Creases.resize(edges);
            }
          }
        });
}

template void Meshed(Auditor<Document> &with);

template void Meshed(Auditor<const Document> &with);
} // namespace DxfIO::Audit
