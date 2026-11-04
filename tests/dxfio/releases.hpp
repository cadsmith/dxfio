#ifndef DXFIO_TESTS_DXF_RELEASES_HPP
#define DXFIO_TESTS_DXF_RELEASES_HPP

#include "dxfio/version.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <string>
#include <string_view>

namespace DxfIO
{
inline constexpr std::array Releases{Version::R2000, Version::R2004, Version::R2007,
                                     Version::R2010, Version::R2013, Version::R2018};

inline auto Fixture(std::string_view stem, Version release) -> std::string
{
  std::string tag{Name(release)};
  std::ranges::transform(tag, tag.begin(),
                         [](unsigned char one) -> char { return static_cast<char>(std::tolower(one)); });
  return std::string{stem} + "-" + tag + ".dxf";
}
} // namespace DxfIO

template <>
struct doctest::StringMaker<DxfIO::Version>
{
  static auto Convert(DxfIO::Version release) -> String { return String{std::string{DxfIO::Name(release)}.c_str()}; }
};

#endif /* DXFIO_TESTS_DXF_RELEASES_HPP */
