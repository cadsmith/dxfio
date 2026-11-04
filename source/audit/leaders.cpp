#include "checks.hpp"
#include "walk.hpp"

#include "dxfio/entities/leader.hpp"
#include "dxfio/entity.hpp"

#include <algorithm>
#include <cstddef>
#include <variant>

namespace DxfIO::Audit
{
namespace
{
// The vertices a leader needs to draw a line.
constexpr std::size_t Fewest = 2;

auto Short(const Entity &one) -> bool
{
  const auto *leader = std::get_if<Entities::Leader>(&one);
  return leader != nullptr && leader->Lead->Points.size() < Fewest;
}
} // namespace

// AutoCAD abandons a file holding a LEADER with fewer than two vertices: such a leader is an error, and left out
// when the checks fix.
template <class DocumentT>
void Led(Auditor<DocumentT> &with)
{
  Lists(*with.Drawing,
        [&with](auto &all) -> void
        {
          for (const Entity &one : all)
          {
            if (Short(one))
            {
              with.Note(Severity::Error, "leader with fewer than two vertices, left out", BaseOf(one).Self);
            }
          }
          if constexpr (Auditor<DocumentT>::Fix)
          {
            std::erase_if(all, Short);
          }
        });
}

template void Led(Auditor<Document> &with);

template void Led(Auditor<const Document> &with);
} // namespace DxfIO::Audit
