#include "file/mapping.hpp"

#include <cstdint>
#include <memory>
#include <utility>

#ifdef _WIN32
// clang-format off
#  define NOMINMAX
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
// clang-format on
#else
#  include <cstdio>
#  include <sys/mman.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif

namespace DxfIO::File
{
#ifdef _WIN32
auto Mapping::Of(const std::filesystem::path &file) -> std::optional<Mapping>
{
  HANDLE opened =
      CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (opened == INVALID_HANDLE_VALUE)
  {
    return std::nullopt;
  }
  LARGE_INTEGER size{};
  const bool plain = GetFileType(opened) == FILE_TYPE_DISK && GetFileSizeEx(opened, &size) != 0 && size.QuadPart > 0;
  std::optional<Mapping> out =
      plain ? Of(reinterpret_cast<std::intptr_t>(opened), static_cast<std::size_t>(size.QuadPart)) : std::nullopt;
  CloseHandle(opened);
  return out;
}

auto Mapping::Of(std::intptr_t descriptor, std::size_t size) -> std::optional<Mapping>
{
  HANDLE mapping = CreateFileMappingW(reinterpret_cast<HANDLE>(descriptor), nullptr, PAGE_READONLY, 0, 0, nullptr);
  if (mapping == nullptr)
  {
    return std::nullopt;
  }
  void *at = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
  CloseHandle(mapping);
  if (at == nullptr)
  {
    return std::nullopt;
  }
  return Mapping{at, size};
}

void Mapping::Release() noexcept
{
  if (m_At != nullptr)
  {
    UnmapViewOfFile(m_At);
  }
}
#else
auto Mapping::Of(const std::filesystem::path &file) -> std::optional<Mapping>
{
  const std::unique_ptr<std::FILE, int (*)(std::FILE *)> opened{std::fopen(file.c_str(), "rb"), &std::fclose};
  if (opened == nullptr)
  {
    return std::nullopt;
  }
  struct stat info{};
  static_cast<void>(::fstat(fileno(opened.get()), &info));
  if (!S_ISREG(info.st_mode) || info.st_size == 0)
  {
    return std::nullopt;
  }
  return Of(fileno(opened.get()), static_cast<std::size_t>(info.st_size));
}

auto Mapping::Of(std::intptr_t descriptor, std::size_t size) -> std::optional<Mapping>
{
  void *at = ::mmap(nullptr, size, PROT_READ, MAP_PRIVATE, static_cast<int>(descriptor), 0);
  if (at == MAP_FAILED)
  {
    return std::nullopt;
  }
  return Mapping{at, size};
}

void Mapping::Release() noexcept
{
  if (m_At != nullptr)
  {
    ::munmap(m_At, m_Size);
  }
}
#endif

Mapping::Mapping(Mapping &&other) noexcept
    : m_At(std::exchange(other.m_At, nullptr)), m_Size(std::exchange(other.m_Size, 0))
{
}

auto Mapping::operator=(Mapping &&other) noexcept -> Mapping &
{
  if (this != &other)
  {
    Release();
    m_At = std::exchange(other.m_At, nullptr);
    m_Size = std::exchange(other.m_Size, 0);
  }
  return *this;
}

Mapping::~Mapping()
{
  Release();
}
} // namespace DxfIO::File
