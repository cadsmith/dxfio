#ifndef DXFIO_TESTS_DXF_DISK_HPP
#define DXFIO_TESTS_DXF_DISK_HPP

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO
{
  inline auto Bytes(const std::filesystem::path &path) -> std::string
  {
    std::ifstream in{path, std::ios::binary};
    return {std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{}};
  }

  inline auto Files(const std::filesystem::path &folder) -> std::vector<std::filesystem::path>
  {
    std::vector<std::filesystem::path> out;
    for (const auto &entry : std::filesystem::directory_iterator{folder})
    {
      out.push_back(entry.path());
    }
    return out;
  }

  inline auto Acad() -> std::filesystem::path
  {
    return std::filesystem::path{DXFIO_FIXTURES_DIR} / "acad";
  }

  inline auto Acad(std::string_view name) -> std::string
  {
    return Bytes(Acad() / name);
  }
} // namespace DxfIO

#endif /* DXFIO_TESTS_DXF_DISK_HPP */
