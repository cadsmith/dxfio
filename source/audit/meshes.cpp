#include "checks.hpp"
#include "walk.hpp"

#include "dxfio/entities/mesh.hpp"
#include "dxfio/entity.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <variant>

namespace DxfIO::Audit
{
  namespace
  {
    /**
     *  Whether a mesh has nothing to draw: no vertices, or no faces.
     */
    auto Empty(const Entities::Mesh &mesh) -> bool
    {
      return mesh.Net->Vertices.empty() || mesh.Net->Faces.empty();
    }

    template <class DocumentT>
    void Creased(Auditor<DocumentT> &with, Part<DocumentT, Entities::Mesh> &mesh)
    {
      const std::size_t edges = mesh.Net->Edges.size() / 2;
      if (mesh.Net->Creases.size() == edges)
      {
        return;
      }
      with.Note(Severity::Warning,
                std::format("mesh with {} creases for {} edges", mesh.Net->Creases.size(), edges),
                mesh.Record.Base.Self);
      if constexpr (Auditor<DocumentT>::Fix)
      {
        mesh.Net->Creases.resize(edges);
      }
    }

    template <class DocumentT>
    void Checked(Auditor<DocumentT> &with, Part<DocumentT, Entities::Mesh> &mesh)
    {
      if (!Empty(mesh))
      {
        Creased(with, mesh);
        return;
      }
      with.Note(Severity::Error,
                std::format("mesh with no {}, left out",
                            mesh.Net->Vertices.empty() ? "vertices" : "faces"),
                mesh.Record.Base.Self);
    }

    auto Hollow(const Entity &one) -> bool
    {
      const auto *mesh = std::get_if<Entities::Mesh>(&one);
      return mesh != nullptr && Empty(*mesh);
    }
  } // namespace

  /**
   *  A MESH with no vertices or no faces is invalid to AutoCAD: an error, and left out when the
   *  checks fix. One with a crease count that is not its edge count is given one crease per edge.
   */
  template <class DocumentT>
  void Meshed(Auditor<DocumentT> &with)
  {
    Lists(*with.Drawing,
          [&with](auto &all) -> void
          {
            for (auto &one : all)
            {
              if (auto *mesh = std::get_if<Entities::Mesh>(&one))
              {
                Checked(with, *mesh);
              }
            }
            if constexpr (Auditor<DocumentT>::Fix)
            {
              std::erase_if(all, Hollow);
            }
          });
  }

  template void Meshed(Auditor<Document> &with);

  template void Meshed(Auditor<const Document> &with);
} // namespace DxfIO::Audit
