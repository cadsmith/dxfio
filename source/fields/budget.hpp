#ifndef DXFIO_FIELDS_BUDGET_HPP
#define DXFIO_FIELDS_BUDGET_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <cstdint>

namespace DxfIO::Fields
{
class DXFIO_API Budget
{
public:
  explicit Budget(std::size_t room);

  auto Claim(std::int64_t wanted) -> std::size_t;

private:
  std::size_t m_Left;
};
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_BUDGET_HPP */
