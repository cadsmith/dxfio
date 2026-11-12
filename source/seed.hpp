#ifndef DXFIO_SEED_HPP
#define DXFIO_SEED_HPP

#include "dxfio/api.hpp"
#include "dxfio/handle.hpp"

#include <cstddef>
#include <vector>

namespace DxfIO
{
  /**
   *  Where new handles come from: one after the other from a first handle, or from the gaps between
   *  the handles in use.
   */
  class DXFIO_API Seed
  {
  public:
    /**
     *  Takes that handle, then each one after it, up to the one before the last handle there is.
     */
    explicit Seed(Handle first = Handle{.Value = 1});

    /**
     *  Takes the lowest handle none of those in use is, then the next lowest, and so on.
     */
    explicit Seed(std::vector<Handle> used);

    auto Take() -> Handle;

    [[nodiscard]] auto Next() const -> Handle;

  private:
    Handle              m_Next;
    std::vector<Handle> m_Used;
    std::size_t         m_Passed{};
  };
} // namespace DxfIO

#endif /* DXFIO_SEED_HPP */
