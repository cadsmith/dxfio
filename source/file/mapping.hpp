#ifndef DXFIO_FILE_MAPPING_HPP
#define DXFIO_FILE_MAPPING_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string_view>

namespace DxfIO::File
{
// A file's bytes as the operating system maps them, read through the view as they lie on disk, with no
// copy into memory of our own; the mapping ends with the object.
class DXFIO_API Mapping
{
public:
  // The file mapped, or nothing for one that cannot be opened, is not a plain file, is empty, or the
  // system will not map; the caller then reads it instead.
  [[nodiscard]] static auto Of(const std::filesystem::path &file) -> std::optional<Mapping>;

  // A file already open mapped by its descriptor, a Windows handle cast to one there; nothing when the
  // system will not map it.
  [[nodiscard]] static auto Of(std::intptr_t descriptor, std::size_t size) -> std::optional<Mapping>;

  Mapping(const Mapping &) = delete;
  Mapping(Mapping &&other) noexcept;
  auto operator=(const Mapping &) -> Mapping & = delete;
  auto operator=(Mapping &&other) noexcept -> Mapping &;
  ~Mapping();

  [[nodiscard]] auto View() const -> std::string_view { return {static_cast<const char *>(m_At), m_Size}; }

private:
  Mapping(void *at, std::size_t size) : m_At(at), m_Size(size) {}

  void Release() noexcept;

  void *m_At{nullptr};
  std::size_t m_Size{};
};
} // namespace DxfIO::File

#endif /* DXFIO_FILE_MAPPING_HPP */
