#include "checks.hpp"
#include "tags/codes.hpp"
#include "walk.hpp"

#include "dxfio/group.hpp"
#include "dxfio/unknown.hpp"

#include <algorithm>
#include <format>
#include <variant>
#include <vector>

namespace DxfIO::Audit
{
namespace
{
auto Undefined(const Group &one) -> bool
{
  return !Tags::Codes::Defined(one.Code);
}

// Reports each group of a code DXF does not define, and leaves it out when the checks fix.
template <class DocumentT, class GroupsT>
void Checked(Auditor<DocumentT> &with, GroupsT &all, Handle self)
{
  for (const Group &one : all)
  {
    if (Undefined(one))
    {
      with.Note(Severity::Error, std::format("group {} is no DXF group, left out", one.Code), self);
    }
  }
  if constexpr (Auditor<DocumentT>::Fix)
  {
    std::erase_if(all, Undefined);
  }
}

template <class DocumentT, class RecordT>
void Loosely(Auditor<DocumentT> &with, RecordT &one)
{
  if (auto *kept = std::get_if<Unknown>(&one))
  {
    Checked(with, kept->Groups, kept->Base.Self);
  }
}
} // namespace

// AutoCAD refuses a file holding a group of a code the DXF reference does not define, "Undefined group code": each
// is an error, in a record's extra groups, its extended data, or a record kept whole, and left out when fixed.
template <class DocumentT>
void Grouped(Auditor<DocumentT> &with)
{
  Bases(*with.Drawing,
        [&with](auto &base) -> void
        {
          Checked(with, base.Extra, base.Self);
          for (auto &data : base.Extended)
          {
            Checked(with, data.Groups, base.Self);
          }
        });
  for (auto &table : with.Drawing->Loose)
  {
    for (auto &one : table.Records)
    {
      Checked(with, one.Groups, one.Base.Self);
    }
  }
  Lists(*with.Drawing,
        [&with](auto &all) -> void
        {
          for (auto &one : all)
          {
            Loosely(with, one);
          }
        });
  for (auto &one : with.Drawing->Objects)
  {
    Loosely(with, one);
  }
}

template void Grouped(Auditor<Document> &with);

template void Grouped(Auditor<const Document> &with);
} // namespace DxfIO::Audit
