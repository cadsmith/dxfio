#include "dxfio/edit/handles.hpp"

#include "seeding.hpp"

namespace DxfIO::Edit
{
  auto NewHandle(Document &drawing) -> std::expected<Handle, Failure>
  {
    return Seeded(drawing).transform(
        [&drawing](Seed handles) -> Handle
        {
          const Handle out = handles.Take();
          Moved(drawing, handles.Next());
          return out;
        });
  }
} // namespace DxfIO::Edit
