#ifndef DXFIO_TESTS_DXF_ENTITIES_HEAVY_HPP
#define DXFIO_TESTS_DXF_ENTITIES_HEAVY_HPP

#include "../disk.hpp"
#include "../releases.hpp"
#include "entities/io.hpp"

#include "dxfio/load.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

namespace DxfIO::Entities
{
inline auto Opened(std::string_view stem, Version release) -> DxfIO::Document
{
  const auto read = Load(Acad(Fixture(stem, release)));
  REQUIRE(read);
  return read->Document;
}

inline auto Heavy(Version release = Version::R2018) -> DxfIO::Document
{
  return Opened("heavy", release);
}

template <class KindT = Polyline>
auto Traced(const List &all, std::uint64_t self) -> const KindT &
{
  const auto found =
      std::ranges::find_if(all, [self](const Entity &one) -> auto { return Based(one).Self.Value == self; });
  REQUIRE(found != all.end());
  const auto *held = std::get_if<KindT>(&*found);
  REQUIRE(held != nullptr);
  return *held;
}

inline auto Drawing(std::string_view records) -> std::string
{
  std::string out = "  0\r\nSECTION\r\n  2\r\nHEADER\r\n  9\r\n$ACADVER\r\n"
                    "  1\r\nAC1032\r\n  0\r\nENDSEC\r\n"
                    "  0\r\nSECTION\r\n  2\r\nENTITIES\r\n";
  out += records;
  out += "  0\r\nENDSEC\r\n  0\r\nEOF\r\n";
  return out;
}
} // namespace DxfIO::Entities

#endif /* DXFIO_TESTS_DXF_ENTITIES_HEAVY_HPP */
