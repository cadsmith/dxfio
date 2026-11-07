#ifndef DXFIO_FIELDS_REPEATS_HPP
#define DXFIO_FIELDS_REPEATS_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <vector>

namespace DxfIO::Fields
{
class DXFIO_API Repeats
{
public:
  void Clear();

  void Asked(std::size_t begin, std::size_t end, int code);

  [[nodiscard]] auto Repeated(std::size_t at, int code) const -> bool;

  void Mark();

  [[nodiscard]] auto Trails(int code) const -> bool;

private:
  struct Ask
  {
    std::size_t Begin{};
    std::size_t End{};
    int Code{};

    auto operator==(const Ask &) const -> bool = default;
  };

  std::vector<Ask> m_Asked;
  std::size_t m_Last{};
};
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_REPEATS_HPP */
