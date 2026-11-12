#ifndef DXFIO_HEAD_HPP
#define DXFIO_HEAD_HPP

#include "fields/context.hpp"
#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/base.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <span>
#include <vector>

namespace DxfIO
{
  inline constexpr int Handled = 5;

  struct DXFIO_API Heading
  {
    Base *Into{};
    int   Code{Handled};
  };

  [[nodiscard]] DXFIO_API auto Head(std::span<const Tags::Tag> body, const Heading &to,
                                    const Fields::Reading &with) -> std::vector<Tags::Tag>;

  [[nodiscard]] DXFIO_API auto Head(Fields::Writing &to, const Base &from, int code)
      -> std::expected<void, Failure>;
} // namespace DxfIO

#endif /* DXFIO_HEAD_HPP */
