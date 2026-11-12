#ifndef DXFIO_HEADER_IO_HPP
#define DXFIO_HEADER_IO_HPP

#include "encoding/codepage.hpp"
#include "fields/context.hpp"
#include "file/section.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/group.hpp"
#include "dxfio/header/header.hpp"
#include "dxfio/version.hpp"

#include <expected>
#include <span>
#include <string>
#include <vector>

namespace DxfIO::Header
{
  [[nodiscard]] DXFIO_API auto Declared(std::span<const File::Variable> all) -> Version;
  [[nodiscard]] DXFIO_API auto Declared(std::span<const Variable> all) -> Version;
  [[nodiscard]] DXFIO_API auto Named(std::span<const File::Variable> all)
      -> const Encoding::Codepage &;
  [[nodiscard]] DXFIO_API auto Named(std::span<const Variable> all) -> const Encoding::Codepage &;
  [[nodiscard]] DXFIO_API auto Opaque(std::span<const File::Variable> all) -> bool;
  [[nodiscard]] DXFIO_API auto Opaque(std::span<const Variable> all) -> bool;
  [[nodiscard]] DXFIO_API auto Read(std::span<const File::Variable> all,
                                    const Fields::Reading          &with) -> std::vector<Variable>;
  [[nodiscard]] DXFIO_API auto Write(Fields::Writing &to, std::span<const Variable> all)
      -> std::expected<void, Failure>;
} // namespace DxfIO::Header

#endif /* DXFIO_HEADER_IO_HPP */
