#ifndef DXFIO_DOWN_DRAWN_HPP
#define DXFIO_DOWN_DRAWN_HPP

#include "seed.hpp"

#include "dxfio/document.hpp"
#include "dxfio/entities/ellipse.hpp"
#include "dxfio/entities/line.hpp"
#include "dxfio/entities/mtext.hpp"
#include "dxfio/entities/polyline.hpp"
#include "dxfio/entities/ray.hpp"
#include "dxfio/entities/record.hpp"
#include "dxfio/entities/spline.hpp"
#include "dxfio/entities/text.hpp"
#include "dxfio/entities/xline.hpp"
#include "dxfio/vector.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Down
{
struct Extents
{
  Vector Low{};
  Vector High{};
};

struct Axes
{
  Vector X{Right};
  Vector Y{.X = 0, .Y = 1, .Z = 0};
  Vector Z{Up};
};

[[nodiscard]] auto Cross(const Vector &one, const Vector &other) -> Vector;

[[nodiscard]] auto Arbitrary(const Vector &normal) -> Axes;

[[nodiscard]] auto Child(const Entities::Record &parent, Seed &handles) -> Entities::Record;

[[nodiscard]] auto Bounded(const Document &from) -> Extents;

[[nodiscard]] auto Traced(const Entities::Ellipse &one, Seed &handles) -> Entities::Polyline;

[[nodiscard]] auto Traced(const Entities::Spline &one, Seed &handles) -> Entities::Polyline;

[[nodiscard]] auto Clipped(const Entities::Ray &one, const Extents &box) -> Entities::Line;

[[nodiscard]] auto Clipped(const Entities::XLine &one, const Extents &box) -> Entities::Line;

[[nodiscard]] auto Plain(std::string_view formatted) -> std::vector<std::string>;

[[nodiscard]] auto Lettered(const Entities::MText &one, Seed &handles) -> std::vector<Entities::Text>;
} // namespace DxfIO::Down

#endif /* DXFIO_DOWN_DRAWN_HPP */
