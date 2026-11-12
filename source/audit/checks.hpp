#ifndef DXFIO_AUDIT_CHECKS_HPP
#define DXFIO_AUDIT_CHECKS_HPP

#include "held.hpp"
#include "seed.hpp"

#include "dxfio/audit/audit.hpp"
#include "dxfio/document.hpp"
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
  template <class DocumentT, class PartT>
  using Part = std::conditional_t<std::is_const_v<DocumentT>, const PartT, PartT>;

  /**
   *  What the checks share as they run: the drawing, read only for Check, which so copies nothing,
   *  or to fix for Repair.
   */
  template <class DocumentT>
  struct Auditor
  {
    /**
     *  Whether the checks fix what they find, which they do only to a drawing they may change.
     */
    static constexpr bool Fix = !std::is_const_v<DocumentT>;

    DocumentT *Drawing{};
    Seed       Handles;
    Report     Out{};

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

  void Localized(Tables::Layer &one, const Document &drawing);

  void Localized(Tables::Dimstyle &one, const Document &drawing);

  void Localized(Tables::Vport &one, const Document &drawing);

  template <class RecordT>
  void Localized(RecordT & /*one*/, const Document & /*drawing*/)
  {
  }

  template <class DocumentT>
  void Seeded(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Handled(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Unnamed(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Tabled(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Applied(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Referred(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Weighted(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Meshed(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Curved(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Led(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Sequenced(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Classed(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Attributed(Auditor<DocumentT> &with);

  template <class DocumentT>
  void Grouped(Auditor<DocumentT> &with);
} // namespace DxfIO::Audit

#endif /* DXFIO_AUDIT_CHECKS_HPP */
