#include "seed.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <utility>

namespace DxfIO
{
  Seed::Seed(Handle first) : m_Next(first) {}

  Seed::Seed(std::vector<Handle> used) : m_Next{.Value = 1}, m_Used(std::move(used))
  {
    std::ranges::sort(m_Used);
  }

  auto Seed::Take() -> Handle
  {
    for (; m_Passed < m_Used.size() && m_Used[m_Passed] <= m_Next; ++m_Passed)
    {
      if (m_Used[m_Passed] == m_Next)
      {
        ++m_Next.Value;
      }
    }
    const Handle taken = m_Next;
    const bool   last  = taken.Value == std::numeric_limits<std::uint64_t>::max();
    m_Next.Value       = last || taken.Value == 0 ? 0 : taken.Value + 1;
    return last ? Handle{} : taken;
  }

  auto Seed::Next() const -> Handle
  {
    return m_Next;
  }
} // namespace DxfIO
