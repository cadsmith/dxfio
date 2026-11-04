#include "entities/shapes/seqend.hpp"

#include "../kind.hpp"

#include <string_view>

namespace DxfIO::Entities
{
TEST_CASE("seqend: the part every entity opens with and nothing else")
{
  constexpr std::string_view text = "  0\r\nSEQEND\r\n  5\r\n2D\r\n330\r\n2A\r\n"
                                    "100\r\nAcDbEntity\r\n  8\r\nwalls\r\n"
                                    " 62\r\n     2\r\n";
  const Seqend taken = Replayed<Seqend>(std::string{text});
  CHECK(taken.Record.Layer == "walls");
  CHECK(taken.Record.Color == 2);
}
} // namespace DxfIO::Entities
