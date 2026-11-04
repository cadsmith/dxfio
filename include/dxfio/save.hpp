#ifndef DXFIO_SAVE_HPP
#define DXFIO_SAVE_HPP

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"
#include "dxfio/version.hpp"

#include <expected>
#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>

namespace DxfIO
{
/// How a drawing is written.
struct DXFIO_API Options
{
  /// The release to write. A drawing of a newer release is narrowed to it, and what it cannot hold is noted.
  Version Release{Version::R2018};
  /// The code page for text before R2007, such as "ANSI_1252"; empty keeps the drawing's $DWGCODEPAGE.
  /// A name no code page has makes Save fail.
  std::string Codepage{};
  /// Writes binary DXF instead of ASCII.
  bool Binary{};
};

/// A drawing written as DXF, with what the writer had to note about it.
struct DXFIO_API Saved
{
  /// The bytes of the file.
  std::string Bytes{};
  /// What the release asked for could not hold, and how it was kept or left out.
  std::vector<Issue> Issues{};
};

/// Writes a drawing as the bytes of a DXF file.
[[nodiscard]] DXFIO_API auto Save(const Document &drawing, const Options &with = {}) -> std::expected<Saved, Failure>;

/// Writes a drawing to a stream.
[[nodiscard]] DXFIO_API auto Save(const Document &drawing, std::ostream &to, const Options &with = {})
    -> std::expected<std::vector<Issue>, Failure>;

/// Writes a drawing to a file, replacing it.
[[nodiscard]] DXFIO_API auto SaveFile(const Document &drawing, const std::filesystem::path &file,
                                      const Options &with = {}) -> std::expected<std::vector<Issue>, Failure>;
} // namespace DxfIO

#endif /* DXFIO_SAVE_HPP */
