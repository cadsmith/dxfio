#include "checks.hpp"
#include "handseed.hpp"
#include "held.hpp"
#include "past.hpp"

#include "dxfio/handle.hpp"

#include <format>

namespace DxfIO::Audit
{
// AutoCAD takes new handles from $HANDSEED, and refuses a drawing whose seed is behind the handles in use or, with
// handles in use, missing: each is an error. The checks that fix move the seed past every handle as they end.
namespace
{
// Whether any record of the drawing holds a handle.
auto Holding(const Document &drawing) -> bool
{
  bool any = false;
  Held(drawing, [&any](Handle one) -> void { any = any || one; });
  return any;
}
} // namespace

template <class DocumentT>
void Seeded(Auditor<DocumentT> &with)
{
  if (!Holding(*with.Drawing))
  {
    return;
  }
  const Handle past = Past(*with.Drawing);
  const Handle seed = ReadSeed(*with.Drawing);
  if (!seed)
  {
    with.Note(Severity::Error, "no $HANDSEED", {});
    return;
  }
  if (past && seed < past)
  {
    with.Note(Severity::Error, std::format("$HANDSEED {} behind the handles in use", Hex(seed)), {});
  }
}

template void Seeded(Auditor<Document> &with);

template void Seeded(Auditor<const Document> &with);
} // namespace DxfIO::Audit
