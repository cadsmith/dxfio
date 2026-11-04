#ifndef DXFIO_TAGS_CODES_HPP
#define DXFIO_TAGS_CODES_HPP

#include "dxfio/api.hpp"

#include <cstdint>

namespace DxfIO::Tags::Codes
{
enum class Type : std::uint8_t
{
  Text,
  Int16,
  Int32,
  Int64,
  Real,
  Boolean,
  Handle,
  Binary,
};

inline constexpr int Last = 1071;

inline constexpr int Marker = 100;

inline constexpr int Embedded = 101;

inline constexpr int Control = 102;

[[nodiscard]] DXFIO_API auto Of(int code) -> Type;

// Whether the DXF reference gives the code a type: AutoCAD refuses a file holding one it does not, and the embedded
// object marker, 101, which it writes itself.
[[nodiscard]] DXFIO_API auto Defined(int code) -> bool;
} // namespace DxfIO::Tags::Codes

#endif /* DXFIO_TAGS_CODES_HPP */
