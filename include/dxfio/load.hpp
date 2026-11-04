#ifndef DXFIO_LOAD_HPP
#define DXFIO_LOAD_HPP

#include "dxfio/api.hpp"
#include "dxfio/document.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <filesystem>
#include <iosfwd>
#include <string_view>
#include <vector>

namespace DxfIO
{
/// A drawing read from DXF, with what the reader had to note about it.
struct DXFIO_API Loaded
{
  /// The drawing, every section and every group the file held.
  DxfIO::Document Document{};
  /// What the reader kept, mended or left out, with the line it found it on.
  std::vector<Issue> Issues{};
};

/// Reads a drawing from the bytes of a DXF file, ASCII or binary, of any release from R10 to R2018.
/// Fails only when the bytes are not DXF at all; anything it can step over becomes an issue.
[[nodiscard]] DXFIO_API auto Load(std::string_view bytes) -> std::expected<Loaded, Failure>;

/// Reads a drawing from a stream, to its end. A stream that has already failed, as a file that did not open, fails.
[[nodiscard]] DXFIO_API auto Load(std::istream &from) -> std::expected<Loaded, Failure>;

/// Reads a drawing from a file.
[[nodiscard]] DXFIO_API auto LoadFile(const std::filesystem::path &file) -> std::expected<Loaded, Failure>;
} // namespace DxfIO

#endif /* DXFIO_LOAD_HPP */
