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

    template <class DrawingT>
    auto Run(DrawingT &drawing) -> Report
    {
      Auditor<DrawingT> with{.Drawing = &drawing, .Handles = Seed{}};
      if constexpr (Auditor<DrawingT>::Fix)
      {
        with.Handles = Seed{Past(drawing)};
      }
      Seeded(with);
      Handled(with);
      Unnamed(with);
      Tabled(with);
      Applied(with);
      Referred(with);
      Weighted(with);
      Meshed(with);
      Curved(with);
      Led(with);
      Sequenced(with);
      Attributed(with);
      Grouped(with);
      Classed(with);
      if constexpr (Auditor<DrawingT>::Fix)
      {
        const Handle past = Past(drawing);
        WriteSeed(drawing, past ? past : Last);
      }
      return std::move(with.Out);
    }
  } // namespace

  auto Report::IsClean() const noexcept -> bool
  {
    return std::ranges::none_of(Findings, Unfixed);
  }

  auto Report::Errors() const noexcept -> std::size_t
  {
    return static_cast<std::size_t>(std::ranges::count(Findings, Severity::Error, &Finding::Level));
  }

  auto Report::Warnings() const noexcept -> std::size_t
  {
    return static_cast<std::size_t>(
        std::ranges::count(Findings, Severity::Warning, &Finding::Level));
  }

  auto Check(const Drawing &drawing) -> Report
  {
    return Run(drawing);
  }

  auto Repair(Drawing &drawing) -> Report
  {
    return Run(drawing);
  }
} // namespace DxfIO::Audit
