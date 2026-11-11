#include "empty/classes.hpp"

#include "checks.hpp"
#include "header/io.hpp"

#include "dxfio/raw.hpp"

#include <format>
#include <string>
#include <utility>
#include <variant>

namespace DxfIO::Audit
{
// AutoCAD declares every kind of its own that a drawing holds in a CLASS record, and readers that need it misread
// a file without one: a kind present with no CLASS is an error, and given the record AutoCAD writes when fixed.
template <class DocumentT>
void Classed(Auditor<DocumentT> &with)
{
  const Version release = Header::Declared(with.Drawing->Header);
  if (release < Version::R13)
  {
    return;
  }
  for (Raw &one : Empty::Undeclared(*with.Drawing, release))
  {
    with.Note(Severity::Error, std::format("no CLASS for {}", std::get<std::string>(one.Groups.front().Value)), {});
    if constexpr (Auditor<DocumentT>::Fix)
    {
      with.Drawing->Classes.push_back(std::move(one));
    }
  }
}

template void Classed(Auditor<Document> &with);

template void Classed(Auditor<const Document> &with);
} // namespace DxfIO::Audit
