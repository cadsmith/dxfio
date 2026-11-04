#include "dxfio/audit/audit.hpp"

#include "checks.hpp"
#include "handseed.hpp"
#include "past.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <utility>

namespace DxfIO::Audit
{
namespace
{
constexpr Handle Last{.Value = std::numeric_limits<std::uint64_t>::max()};

auto Unfixed(const Finding &one) -> bool
{
  return one.Level == Severity::Error && !one.Fixed;
}

template <class DocumentT>
auto Run(DocumentT &drawing) -> Report
{
  Auditor<DocumentT> with{.Drawing = &drawing, .Handles = Seed{}};
  if constexpr (Auditor<DocumentT>::Fix)
  {
    with.Handles = Seed{Past(drawing)};
  }
  Handled(with);
  Tabled(with);
  Applied(with);
  Referred(with);
  Meshed(with);
  Curved(with);
  Sequenced(with);
  if constexpr (Auditor<DocumentT>::Fix)
  {
    const Handle past = Past(drawing);
    WriteSeed(drawing, past ? past : Last);
  }
  return std::move(with.Out);
}
} // namespace

auto Report::Clean() const -> bool
{
  return std::ranges::none_of(Findings, Unfixed);
}

auto Report::Errors() const -> std::size_t
{
  return static_cast<std::size_t>(std::ranges::count(Findings, Severity::Error, &Finding::Level));
}

auto Report::Warnings() const -> std::size_t
{
  return static_cast<std::size_t>(std::ranges::count(Findings, Severity::Warning, &Finding::Level));
}

auto Check(const Document &drawing) -> Report
{
  return Run(drawing);
}

auto Repair(Document &drawing) -> Report
{
  return Run(drawing);
}
} // namespace DxfIO::Audit
