#ifndef DXFIO_EMPTY_STYLES_HPP
#define DXFIO_EMPTY_STYLES_HPP

#include "roots.hpp"
#include "seed.hpp"

#include "dxfio/objects/material.hpp"
#include "dxfio/objects/mleader/style.hpp"
#include "dxfio/objects/mline/style.hpp"
#include "dxfio/objects/table/style.hpp"

#include <vector>

namespace DxfIO::Empty
{
auto Materials(const Roots &ids) -> std::vector<Objects::Material>;

auto Mleaderstyle(const Roots &ids, Seed &handles) -> Objects::MLeaderStyle;

auto Mlinestyle(Seed &handles) -> Objects::MLineStyle;

auto Tablestyle(Seed &handles) -> Objects::TableStyle;
} // namespace DxfIO::Empty

#endif /* DXFIO_EMPTY_STYLES_HPP */
