#ifndef DXFIO_AUDIT_CHECKS_HPP
#define DXFIO_AUDIT_CHECKS_HPP

#include "held.hpp"
#include "seed.hpp"

#include "dxfio/audit/audit.hpp"
#include "dxfio/drawing.hpp"
#include "dxfio/handle.hpp"

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace DxfIO::Audit
{
  /**
   *  A part of the drawing as the checks reach it: read only when they check, open to change when
   *  they repair.
   */
  template <class DrawingT, class PartT>
  using Part = std::conditional_t<std::is_const_v<DrawingT>, const PartT, PartT>;

  /**
   *  What the checks share as they run: the drawing, read only for Check, which so copies nothing,
   *  or to fix for Repair.
   */
  template <class DrawingT>
  struct Auditor
  {
    /**
     *  Whether the checks fix what they find, which they do only to a drawing they may change.
     */
    static constexpr bool Fix = !std::is_const_v<DrawingT>;

    DrawingT *Drawing{};
    Seed      Handles;
    Report    Out{};

    /**
     *  Adds a finding, fixed when the checks fix.
     */
    void Note(Severity level, std::string what, Handle subject)
    {
      Out.Findings.push_back(
          Finding{.Level = level, .What = std::move(what), .Subject = subject, .Fixed = Fix});
    }

    /**
     *  Takes a handle no record holds: past every handle in use, then, once none is left past them,
     *  the lowest free one.
     */
    auto Take() -> Handle
    {
      const Handle out = Handles.Take();
      if (out)
      {
        return out;
      }
      std::vector<Handle> used;
      Held(*Drawing, [&used](Handle one) -> void { used.push_back(one); });
      Handles = Seed{std::move(used)};
      return Handles.Take();
    }
  };

  void Localized(Tables::Layer &one, const Drawing &drawing);

  void Localized(Tables::DimStyle &one, const Drawing &drawing);

  void Localized(Tables::VPort &one, const Drawing &drawing);

  template <class RecordT>
  void Localized(RecordT & /*one*/, const Drawing & /*drawing*/)
  {
  }

  template <class DrawingT>
  void Seeded(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Handled(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Unnamed(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Tabled(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Applied(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Referred(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Weighted(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Meshed(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Curved(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Led(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Sequenced(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Classed(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Attributed(Auditor<DrawingT> &with);

  template <class DrawingT>
  void Grouped(Auditor<DrawingT> &with);
} // namespace DxfIO::Audit

#endif /* DXFIO_AUDIT_CHECKS_HPP */
