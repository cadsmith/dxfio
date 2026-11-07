#ifndef DXFIO_PARALLEL_HPP
#define DXFIO_PARALLEL_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <functional>
#include <vector>

namespace DxfIO
{
// A run of items, from Begin to End exclusive.
struct DXFIO_API Range
{
  std::size_t Begin{};
  std::size_t End{};

  auto operator==(const Range &) const -> bool = default;
};

// The threads to work with: the count asked for, or the machine's when none or less is asked, one at least.
[[nodiscard]] DXFIO_API auto Threads(int asked) -> int;

// How many parts to cut work for that many threads into: a few each, so that the uneven ones even out.
[[nodiscard]] DXFIO_API auto Parts(int threads) -> std::size_t;

// Count items cut into at most that many parts of near equal size, none shorter than least but the last;
// one part, or none for no items.
[[nodiscard]] DXFIO_API auto Parted(std::size_t count, std::size_t parts, std::size_t least) -> std::vector<Range>;

// Runs the job for each index below jobs, on the caller's thread alone when one thread is given, otherwise
// on it and up to threads minus one more, each taking the next index. An exception a job throws reaches
// the caller once every thread has stopped.
DXFIO_API void Spread(std::size_t jobs, int threads, const std::function<void(std::size_t)> &job);
} // namespace DxfIO

#endif /* DXFIO_PARALLEL_HPP */
