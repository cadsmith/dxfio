#include "objects/shapes/underlay/definition.hpp"

#include "../../kind.hpp"
#include "objects/io.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Objects
{
namespace
{
auto Text(std::string_view type, std::string_view file, std::string_view name) -> std::string
{
  return "  0\r\n" + std::string{type} +
         "\r\n  5\r\n325\r\n102\r\n{ACAD_REACTORS\r\n330\r\n324\r\n102\r\n}"
         "\r\n330\r\n324\r\n100\r\nAcDbUnderlayDefinition\r\n  1\r\n" +
         std::string{file} + "\r\n  2\r\n" + std::string{name} + "\r\n";
}
} // namespace

TEST_CASE("underlay definition: the file and what of it is shown")
{
  const std::string text = Text("PDFDEFINITION", "leve.pdf", "1");
  const PdfDefinition one = DxfIO::Read<PdfDefinition>(text);
  CHECK(one.File == "leve.pdf");
  CHECK(one.Name == "1");
  CHECK(one.Base.Owner == Handle{.Value = 0x324});
  CHECK(Written(one) == text);
}

TEST_CASE("underlay definition: each format under its own type")
{
  const std::string dwf = Text("DWFDEFINITION", "reseau.dwf", "Reseau");
  CHECK(Written(DxfIO::Read<DwfDefinition>(dwf)) == dwf);
  const std::string dgn = Text("DGNDEFINITION", "voirie.dgn", "Default");
  CHECK(Written(DxfIO::Read<DgnDefinition>(dgn)) == dgn);
  CHECK(Objects::Type(Object{PdfDefinition{}}) == "PDFDEFINITION");
  CHECK(Objects::Type(Object{DwfDefinition{}}) == "DWFDEFINITION");
  CHECK(Objects::Type(Object{DgnDefinition{}}) == "DGNDEFINITION");
}
} // namespace DxfIO::Objects
