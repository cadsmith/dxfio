#include "objects/shapes/material.hpp"

#include "../kind.hpp"

#include "dxfio/matrix.hpp"
#include "dxfio/version.hpp"

#include <string_view>

namespace DxfIO::Objects
{
TEST_CASE("material: AutoCAD's Global, its four maps placed and what R2007 added")
{
  constexpr std::string_view text = "  0\r\nMATERIAL\r\n"
                                    "  5\r\nEE\r\n"
                                    "102\r\n{ACAD_XDICTIONARY\r\n"
                                    "360\r\n173\r\n"
                                    "102\r\n}\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n6A\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n6A\r\n"
                                    "100\r\nAcDbMaterial\r\n"
                                    "  1\r\nGlobal\r\n"
                                    " 43\r\n0.0007999999797903\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0007999999797903\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n1.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n1.0\r\n"
                                    " 49\r\n0.0007999999797903\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0007999999797903\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n1.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n1.0\r\n"
                                    "142\r\n0.0007999999797903\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0007999999797903\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n1.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n1.0\r\n"
                                    "144\r\n0.0007999999797903\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0007999999797903\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n1.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n1.0\r\n"
                                    "273\r\n     1\r\n"
                                    "148\r\n0.25\r\n"
                                    "149\r\n0.125\r\n"
                                    "468\r\n0.375\r\n"
                                    " 93\r\n        1\r\n"
                                    " 94\r\n       17\r\n"
                                    "282\r\n     1\r\n"
                                    "1001\r\nACAD\r\n"
                                    "1070\r\n    -1\r\n"
                                    "1070\r\n     3\r\n"
                                    "1070\r\n     0\r\n"
                                    "1000\r\n\r\n"
                                    "1071\r\n        0\r\n"
                                    "1070\r\n     0\r\n";
  const Material taken = Read<Material>(text);
  CHECK(taken.Name == "Global");
  CHECK(taken.Maps->Diffuse.Transform.Cells.front() == 0.0007999999797903);
  CHECK(taken.Maps->Specular.Transform == Identity);
  CHECK(taken.Illumination == 0.125);
  CHECK(taken.Reflectivity == 0.375);
  CHECK(taken.Channels == 17);
  CHECK(taken.Base.Extra.empty());
  CHECK(Written(taken) == text);
}

TEST_CASE("material: a material's groups written when off what a new one holds")
{
  constexpr std::string_view text = "  0\r\nMATERIAL\r\n"
                                    "  5\r\nEC\r\n"
                                    "102\r\n{ACAD_XDICTIONARY\r\n"
                                    "360\r\n1F7\r\n"
                                    "102\r\n}\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n6A\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n6A\r\n"
                                    "100\r\nAcDbMaterial\r\n"
                                    "  1\r\nByLayer\r\n"
                                    "  2\r\nprobe\r\n"
                                    " 40\r\n0.5\r\n"
                                    " 90\r\n-1023410177\r\n"
                                    " 42\r\n0.5\r\n"
                                    "  3\r\na.png\r\n"
                                    " 73\r\n     1\r\n"
                                    " 79\r\n     2\r\n"
                                    "172\r\n     3\r\n"
                                    "174\r\n     2\r\n"
                                    "273\r\n     1\r\n"
                                    " 94\r\n       63\r\n";
  const Material taken = Read<Material>(text);
  CHECK(taken.Description == "probe");
  CHECK(taken.Ambient.Factor == 0.5);
  CHECK(taken.Ambient.Method == 0);
  CHECK(taken.Ambient.Color == -1023410177);
  CHECK(taken.Maps->Diffuse.File == "a.png");
  CHECK(taken.Maps->Diffuse.Projection == 1);
  CHECK(taken.Maps->Diffuse.Blend == 0.5);
  CHECK(taken.Maps->Specular.Tiling == 2);
  CHECK(taken.Maps->Reflection.Projection == 3);
  CHECK(taken.Maps->Reflection.Automatic == 2);
  CHECK(taken.Maps->Refraction.Source == Bitmap);
  CHECK(taken.Base.Extra.empty());
  CHECK(Written(taken) == text);
}

TEST_CASE("material: channels held though none is on, written as 0")
{
  constexpr std::string_view text = "  0\r\nMATERIAL\r\n"
                                    "  5\r\nED\r\n"
                                    "102\r\n{ACAD_XDICTIONARY\r\n"
                                    "360\r\n1F9\r\n"
                                    "102\r\n}\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n6A\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n6A\r\n"
                                    "100\r\nAcDbMaterial\r\n"
                                    "  1\r\nByBlock\r\n"
                                    "  3\r\nb.png\r\n"
                                    "273\r\n     1\r\n"
                                    " 94\r\n        0\r\n";
  const Material taken = Read<Material>(text);
  CHECK(taken.Channels == 0);
  CHECK(taken.Maps->Diffuse.Projection == Box);
  CHECK(Written(taken) == text);
}

TEST_CASE("material: a tiled floor, its maps drawn from images or the scene")
{
  constexpr std::string_view text = "  0\r\nMATERIAL\r\n"
                                    "  5\r\n2CC\r\n"
                                    "102\r\n{ACAD_XDICTIONARY\r\n"
                                    "360\r\n316\r\n"
                                    "102\r\n}\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n6A\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n6A\r\n"
                                    "100\r\nAcDbMaterial\r\n"
                                    "  1\r\nFloor\r\n"
                                    "  2\r\nbrown square tiles\r\n"
                                    " 70\r\n     1\r\n"
                                    " 90\r\n-1023410177\r\n"
                                    " 71\r\n     1\r\n"
                                    " 91\r\n-1023410177\r\n"
                                    "  3\r\ntile.png\r\n"
                                    " 44\r\n0.85\r\n"
                                    " 76\r\n     1\r\n"
                                    " 92\r\n-1023410177\r\n"
                                    " 77\r\n     0\r\n"
                                    "171\r\n     0\r\n"
                                    "175\r\n     0\r\n"
                                    "143\r\n0.55\r\n"
                                    "  8\r\ntile.bump.png\r\n"
                                    "145\r\n1.399999999999999\r\n";
  const Material taken = Read<Material>(text, Version::R2004);
  CHECK(taken.Description == "brown square tiles");
  CHECK(taken.Gloss == 0.85);
  CHECK(taken.Specular.Method == 1);
  CHECK(taken.Maps->Specular.Source == Scene);
  CHECK(taken.Maps->Bump.Blend == 0.55);
  CHECK(taken.Maps->Bump.File == "tile.bump.png");
  CHECK(taken.Maps->Refraction.Source == Scene);
  CHECK(taken.Base.Extra.empty());
  CHECK(Written(taken, Version::R2004) == text);
}

TEST_CASE("material: R2004's Global, without what R2007 added")
{
  constexpr std::string_view text = "  0\r\nMATERIAL\r\n"
                                    "  5\r\nEE\r\n"
                                    "102\r\n{ACAD_XDICTIONARY\r\n"
                                    "360\r\n173\r\n"
                                    "102\r\n}\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n6A\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n6A\r\n"
                                    "100\r\nAcDbMaterial\r\n"
                                    "  1\r\nGlobal\r\n"
                                    " 43\r\n0.0007999999797903\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0007999999797903\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n1.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n0.0\r\n"
                                    " 43\r\n1.0\r\n"
                                    " 49\r\n0.0007999999797903\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0007999999797903\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n1.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n0.0\r\n"
                                    " 49\r\n1.0\r\n"
                                    "142\r\n0.0007999999797903\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0007999999797903\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n1.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n0.0\r\n"
                                    "142\r\n1.0\r\n"
                                    "144\r\n0.0007999999797903\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0007999999797903\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n1.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n0.0\r\n"
                                    "144\r\n1.0\r\n"
                                    "273\r\n     1\r\n"
                                    "1001\r\nACAD\r\n"
                                    "1070\r\n    -1\r\n"
                                    "1070\r\n     3\r\n"
                                    "1070\r\n     0\r\n"
                                    "1000\r\n\r\n"
                                    "1071\r\n        0\r\n"
                                    "1070\r\n     0\r\n";
  const Material taken = Read<Material>(text, Version::R2004);
  CHECK_FALSE(taken.Channels);
  CHECK(taken.Illumination == 0);
  CHECK(Written(taken, Version::R2004) == text);
}

TEST_CASE("material: one AutoCAD read from R2004, written in R2018 without channels")
{
  constexpr std::string_view text = "  0\r\nMATERIAL\r\n"
                                    "  5\r\nED\r\n"
                                    "102\r\n{ACAD_XDICTIONARY\r\n"
                                    "360\r\n1F9\r\n"
                                    "102\r\n}\r\n"
                                    "102\r\n{ACAD_REACTORS\r\n"
                                    "330\r\n6A\r\n"
                                    "102\r\n}\r\n"
                                    "330\r\n6A\r\n"
                                    "100\r\nAcDbMaterial\r\n"
                                    "  1\r\nByBlock\r\n"
                                    "  3\r\nb.png\r\n"
                                    "273\r\n     1\r\n";
  const Material taken = Read<Material>(text);
  CHECK_FALSE(taken.Channels);
  CHECK(Written(taken) == text);
}
} // namespace DxfIO::Objects
