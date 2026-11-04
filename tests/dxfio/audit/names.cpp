#include "audit/names.hpp"

#include "dxfio/create.hpp"

#include <doctest/doctest.h>

namespace DxfIO::Audit
{
TEST_CASE("audit: the names of a table and of the blocks are found without case")
{
  const Document drawing = Create(Version::R2018);
  const Names linetypes = Named(drawing.Ltype);
  CHECK(linetypes.size() == drawing.Ltype.Records.size());
  CHECK(linetypes.contains("CONTINUOUS"));
  CHECK(linetypes.contains("byblock"));
  CHECK_FALSE(linetypes.contains("Continuou"));
  const Names blocks = Defined(drawing.Blocks);
  CHECK(blocks.contains("*MODEL_SPACE"));
  CHECK_FALSE(blocks.contains("Door"));
  CHECK(Folded{}("Walls") == Folded{}("wALLS"));
  CHECK(Alike{}("Walls", "WALLS"));
  CHECK_FALSE(Alike{}("Walls", "Halls"));
}
} // namespace DxfIO::Audit
