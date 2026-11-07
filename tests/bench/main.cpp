// Times the library on one drawing, read into memory first so the disk is out of the figures: load, ASCII
// save, binary save and binary load, the best of several runs each, with the peak memory of the process.
#include "dxfio/dxfio.hpp"

#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <expected>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>

#if defined(_WIN32)
// clang-format off
#  include <windows.h>
#  include <psapi.h>
// clang-format on
#else
#  include <sys/resource.h>
#endif

namespace
{
constexpr double Mega = 1024.0 * 1024.0;

constexpr int Runs = 3;

auto Peak() -> double
{
#if defined(_WIN32)
  PROCESS_MEMORY_COUNTERS counters{};
  GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof counters);
  return static_cast<double>(counters.PeakWorkingSetSize) / Mega;
#elif defined(__APPLE__)
  rusage usage{};
  getrusage(RUSAGE_SELF, &usage);
  return static_cast<double>(usage.ru_maxrss) / Mega;
#else
  rusage usage{};
  getrusage(RUSAGE_SELF, &usage);
  return static_cast<double>(usage.ru_maxrss) / 1024.0;
#endif
}

// The least time of the runs of a step, printed with the throughput it gives over the bytes handled.
template <class StepT>
void Timed(std::string_view what, std::size_t bytes, int runs, StepT &&step)
{
  double best = 0.0;
  for (int run = 0; run < runs; ++run)
  {
    const auto start = std::chrono::steady_clock::now();
    step();
    const double took = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    best = run == 0 ? took : std::min(best, took);
  }
  std::printf("%-12.*s %8.3f s %8.1f MB/s   peak %6.0f MB\n", static_cast<int>(what.size()), what.data(), best,
              static_cast<double>(bytes) / best / Mega, Peak());
}

auto Count(std::string_view text) -> int
{
  int value = Runs;
  std::from_chars(text.data(), text.data() + text.size(), value);
  return value;
}
} // namespace

auto main(int argc, char **argv) -> int
{
  if (argc < 2)
  {
    std::fprintf(stderr, "usage: dxfio_bench <drawing.dxf> [runs]\n");
    return 2;
  }
  std::ifstream in{argv[1], std::ios::binary};
  const std::string bytes{std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{}};
  if (bytes.empty())
  {
    std::fprintf(stderr, "%s: not read\n", argv[1]);
    return 2;
  }
  const int runs = argc > 2 ? Count(argv[2]) : Runs;
  std::printf("%s: %.1f MB, best of %d runs\n", argv[1], static_cast<double>(bytes.size()) / Mega, runs);
  std::expected<DxfIO::Loaded, DxfIO::Failure> loaded;
  Timed("load", bytes.size(), runs, [&] -> void { loaded = DxfIO::Load(bytes); });
  if (!loaded)
  {
    std::fprintf(stderr, "load: %s\n", loaded.error().What.c_str());
    return 1;
  }
  std::printf("  %zu entities, %zu objects, %zu issues\n", loaded->Document.Entities.size(),
              loaded->Document.Objects.size(), loaded->Issues.size());
  std::expected<DxfIO::Saved, DxfIO::Failure> ascii;
  Timed("save", bytes.size(), runs, [&] -> void { ascii = DxfIO::Save(loaded->Document); });
  if (!ascii)
  {
    std::fprintf(stderr, "save: %s\n", ascii.error().What.c_str());
    return 1;
  }
  std::expected<DxfIO::Saved, DxfIO::Failure> binary;
  Timed("save binary", bytes.size(), runs,
        [&] -> void { binary = DxfIO::Save(loaded->Document, DxfIO::Options{.Binary = true}); });
  if (!binary)
  {
    std::fprintf(stderr, "save binary: %s\n", binary.error().What.c_str());
    return 1;
  }
  std::printf("  %.1f MB as ASCII, %.1f MB as binary\n", static_cast<double>(ascii->Bytes.size()) / Mega,
              static_cast<double>(binary->Bytes.size()) / Mega);
  Timed("load binary", binary->Bytes.size(), runs, [&] -> void { loaded = DxfIO::Load(binary->Bytes); });
  return loaded ? 0 : 1;
}
