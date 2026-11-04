#ifndef DXFIO_SHAPES_HPP
#define DXFIO_SHAPES_HPP

namespace DxfIO
{
template <class KindT>
struct Unfiled;

template <class KindT>
constexpr auto Shapes = Unfiled<KindT>{};

template <class KindT>
constexpr auto Type = Unfiled<KindT>{};
} // namespace DxfIO

#endif /* DXFIO_SHAPES_HPP */
