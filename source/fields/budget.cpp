#include "fields/budget.hpp"

#include <utility>

namespace DxfIO::Fields
{
  Budget::Budget(std::size_t room) : m_Left(room) {}

  auto Budget::Claim(std::int64_t wanted) -> std::size_t
  {
    if (wanted <= 0)
    {
      return 0;
    }
    const std::size_t given =
        std::cmp_less(wanted, m_Left) ? static_cast<std::size_t>(wanted) : m_Left;
    m_Left -= given;
    return given;
  }
} // namespace DxfIO::Fields
