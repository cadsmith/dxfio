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
} // namespace DxfIO::Tags::Codes

#endif /* DXFIO_TAGS_CODES_HPP */
