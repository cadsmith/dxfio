#include "file/mapping.hpp"

#include <doctest/doctest.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <utility>

namespace DxfIO::File
{
namespace
{
auto Bytes(const std::filesystem::path &file) -> std::string
{
  std::ifstream in{file, std::ios::binary};
  return {std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{}};
}
} // namespace

TEST_CASE("mapping: a plain file's bytes, as read, through a mapping that moves and ends")
{
  const std::filesystem::path file = std::filesystem::path{DXFIO_FUZZ_DIR} / "corpus" / "sample-r2018.dxf";
  std::optional<Mapping> mapped = Mapping::Of(file);
  REQUIRE(mapped);
  CHECK(mapped->View() == Bytes(file));
  Mapping moved{std::move(*mapped)};
  CHECK(moved.View() == Bytes(file));
  std::optional<Mapping> other = Mapping::Of(file);
  REQUIRE(other);
  *other = std::move(moved);
  CHECK(other->View() == Bytes(file));
  Mapping &same = *other;
  *other = std::move(same);
  CHECK(other->View() == Bytes(file));
}

TEST_CASE("mapping: a file not there, a directory, an empty file and a descriptor the system refuses map to nothing")
{
  CHECK_FALSE(Mapping::Of(std::filesystem::path{DXFIO_FUZZ_DIR} / "not-there.dxf"));
  CHECK_FALSE(Mapping::Of(std::filesystem::path{DXFIO_FUZZ_DIR} / "corpus"));
  const std::filesystem::path empty = std::filesystem::temp_directory_path() / "dxfio-empty.dxf";
  {
    std::ofstream{empty, std::ios::binary};
  }
  CHECK_FALSE(Mapping::Of(empty));
  std::filesystem::remove(empty);
  CHECK_FALSE(Mapping::Of(std::intptr_t{-1}, 16));
}
} // namespace DxfIO::File
