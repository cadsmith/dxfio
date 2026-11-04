#ifndef DXFIO_AUDIT_AUDIT_HPP
#define DXFIO_AUDIT_AUDIT_HPP

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/handle.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace DxfIO::Audit
{
/// How much a finding matters to AutoCAD.
enum class Severity : std::uint8_t
{
  /// AutoCAD opens the drawing as it is.
  Warning,
  /// AutoCAD refuses the drawing, or recovers it.
  Error,
};

/// One thing an audit found wrong in a drawing.
struct DXFIO_API Finding
{
  /// Whether AutoCAD would open the drawing with it.
  Severity Level{};
  /// What is wrong, in a few words, such as "layer walls missing".
  std::string What{};
  /// The record found at fault, or the table a needed record is missing from; none when it has no handle.
  Handle Subject{};
  /// Whether Repair fixed it; a Check leaves it false.
  bool Fixed{};

  auto operator==(const Finding &) const -> bool = default;
};

/// What an audit found in a drawing.
struct DXFIO_API Report
{
  /// The findings, in the order the checks ran: handles, required records, applications, references, meshes,
  /// ellipses, SEQENDs.
  std::vector<Finding> Findings{};

  /// Whether no error is left unfixed: AutoCAD would open the drawing without recovering it.
  [[nodiscard]] auto Clean() const -> bool;

  /// The number of findings that are errors, fixed or not.
  [[nodiscard]] auto Errors() const -> std::size_t;

  /// The number of findings that are warnings, fixed or not.
  [[nodiscard]] auto Warnings() const -> std::size_t;
};

/// Lists what AutoCAD would complain about in a drawing, leaving it unchanged.
[[nodiscard]] DXFIO_API auto Check(const Document &drawing) -> Report;

/// Runs the checks of Check and fixes what they find, in place: a missing or duplicate handle is replaced, a missing
/// layer or required record made, a layer with no plot style given the drawing's default one, an application named
/// by extended data registered, a missing linetype or text style set to ByLayer or Standard, an insert of a missing
/// block left out, a mesh with no vertices or faces left out, one given one crease per edge, an ellipse's ratio
/// brought between 0 and 1,
/// a POLYLINE or an INSERT with attributes given the SEQEND it lacks, an INSERT's attributes said to follow it.
/// A handle it gives comes past every handle in use, or, once the drawing holds the last handle there is, is the lowest
/// one no record holds. It then moves $HANDSEED past the handles in use, adding it when the header has none.
DXFIO_API auto Repair(Document &drawing) -> Report;
} // namespace DxfIO::Audit

#endif /* DXFIO_AUDIT_AUDIT_HPP */
