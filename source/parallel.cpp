#include "parallel.hpp"

#include <algorithm>
#include <atomic>
#include <exception>
#include <mutex>
#include <thread>
#include <utility>

namespace DxfIO
{
  namespace
  {
    constexpr std::size_t Each = 4;

    /**
     *  Runs jobs from the shared counter until none is left, keeping the first exception thrown.
     */
    struct Worker
    {
      const std::function<void(std::size_t)> *Job{};
      std::size_t                             Jobs{};
      std::atomic<std::size_t>               *Next{};
      std::mutex                             *Lock{};
      std::exception_ptr                     *Thrown{};

      void operator()() const noexcept
      {
        for (std::size_t index = Next->fetch_add(1); index < Jobs; index = Next->fetch_add(1))
        {
          try
          {
            (*Job)(index);
          }
          catch (...)
          {
            const std::scoped_lock guard{*Lock};
            if (*Thrown == nullptr)
            {
              *Thrown = std::current_exception();
            }
            return;
          }
        }
      }
    };
  } // namespace

  auto Threads(int asked) -> int
  {
    if (asked > 0)
    {
      return asked;
    }
    return std::max(1, static_cast<int>(std::thread::hardware_concurrency()));
  }

  auto Parts(int threads) -> std::size_t
  {
    const auto count = static_cast<std::size_t>(Threads(threads));
    return count == 1 ? 1 : count * Each;
  }

  auto Parted(std::size_t count, std::size_t parts, std::size_t least) -> std::vector<Range>
  {
    std::vector<Range> out;
    if (count == 0)
    {
      return out;
    }
    const std::size_t wanted =
        std::max<std::size_t>(1, std::min(parts, count / std::max<std::size_t>(1, least)));
    const std::size_t each = (count + wanted - 1) / wanted;
    for (std::size_t begin = 0; begin < count; begin += each)
    {
      out.push_back(Range{.Begin = begin, .End = std::min(count, begin + each)});
    }
    return out;
  }

  void Spread(std::size_t jobs, int threads, const std::function<void(std::size_t)> &job)
  {
    const auto helpers =
        static_cast<std::size_t>(std::max(0, std::min(threads, static_cast<int>(jobs)) - 1));
    if (helpers == 0)
    {
      for (std::size_t index = 0; index < jobs; ++index)
      {
        job(index);
      }
      return;
    }
    std::atomic<std::size_t> next{0};
    std::mutex               lock;
    std::exception_ptr       thrown;
    const Worker worker{.Job = &job, .Jobs = jobs, .Next = &next, .Lock = &lock, .Thrown = &thrown};
    {
      std::vector<std::jthread> pool;
      pool.reserve(helpers);
      for (std::size_t at = 0; at < helpers; ++at)
      {
        pool.emplace_back(worker);
      }
      worker();
    }
    if (thrown != nullptr)
    {
      std::rethrow_exception(thrown);
    }
  }
} // namespace DxfIO
