#ifndef DXFIO_MASKED_HPP
#define DXFIO_MASKED_HPP

namespace DxfIO
{
[[nodiscard]] constexpr auto Masked(int flags, int mask) noexcept -> int
{
  return static_cast<int>(static_cast<unsigned int>(flags) & static_cast<unsigned int>(mask));
}
} // namespace DxfIO

#endif /* DXFIO_MASKED_HPP */
