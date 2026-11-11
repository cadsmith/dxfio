#ifndef DXFIO_EMPTY_CLASSES_HPP
#define DXFIO_EMPTY_CLASSES_HPP

#include "dxfio/document.hpp"
#include "dxfio/object.hpp"
#include "dxfio/raw.hpp"
#include "dxfio/version.hpp"

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace DxfIO::Empty
{
// The CLASS record AutoCAD writes for a kind: its names, its proxy flags, and the releases it writes it in.
struct Class
{
  std::string_view Name;
  std::string_view Program;
  std::string_view Application;
  std::int64_t Flags{};
  bool Entity{};
  Version Since{Version::R13};
  Version Until{Version::R2018};
};

// The CLASS a release writes for a kind of that name, none for a kind it needs no CLASS for.
[[nodiscard]] auto Known(std::string_view name, Version release) -> const Class *;

// The CLASS record of a kind as AutoCAD writes it, counting that many instances from R2004.
[[nodiscard]] auto Declared(const Class &one, Version release, std::int64_t count) -> Raw;

// The CLASS records of the kinds among the objects, for Create.
[[nodiscard]] auto Classes(Version release, std::span<const Object> all) -> std::vector<Raw>;

// The CLASS records a release needs for the kinds the drawing holds, in the order they are first met, that none of
// its own declares, each counting that kind's instances.
[[nodiscard]] auto Undeclared(const Document &drawing, Version release) -> std::vector<Raw>;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_CLASSES_HPP */
