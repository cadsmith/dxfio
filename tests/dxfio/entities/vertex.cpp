#include "entities/shapes/vertex.hpp"

#include "../kind.hpp"
#include "heavy.hpp"

#include <array>
#include <string>
#include <utility>

namespace DxfIO::Entities
{
namespace
{
auto Flagged(int flags) -> Vertex
{
  Vertex out{};
  out.Flags = flags;
  return out;
}

auto Moved(const Vertex &one, Version release, Version from) -> std::string
{
  Fields::Sheet page{release};
  page.To.From = from;
  REQUIRE(Given(page.To, one));
  return page.Out;
}
} // namespace

TEST_CASE("vertex: a curve's, its bulge and its flags")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x2fa);
  REQUIRE(taken.Vertices.Children.size() >= 2);
  CHECK(taken.Vertices.Children.front().Bulge > 0);
  CHECK(taken.Vertices.Children.at(1).Flags == 1);
}

TEST_CASE("vertex: a spline's frame and its fitted points")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x303);
  REQUIRE(taken.Vertices.Children.size() >= 2);
  CHECK(taken.Vertices.Children.at(0).Flags == 16);
  CHECK(taken.Vertices.Children.at(1).Flags == 8);
}

TEST_CASE("vertex: an identifier before its flags, a tangent after them")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x48a);
  REQUIRE(taken.Vertices.Children.size() == 2);
  const Vertex &first = taken.Vertices.Children.front();
  CHECK(first.Start == 1.0);
  CHECK(first.End == 2.0);
  CHECK(first.Bulge == 0.5);
  CHECK(first.Id == 7);
  CHECK(first.Flags == 2);
  CHECK(first.Tangent > 28);
  const std::string text = Written(first);
  CHECK(text.find(" 42\r\n") < text.find(" 91\r\n"));
  CHECK(text.find(" 91\r\n") < text.find(" 70\r\n"));
  CHECK(text.find(" 70\r\n") < text.find(" 50\r\n"));
}

TEST_CASE("vertex: its identifier not written below R2010, but in the release it came from")
{
  const DxfIO::Document file = Heavy();
  const Vertex &first = Traced(file.Entities, 0x48a).Vertices.Children.front();
  REQUIRE(first.Id == 7);
  for (const Version release : {Version::R2000, Version::R2004, Version::R2007})
  {
    CAPTURE(release);
    CHECK_FALSE(Moved(first, release, Version::R2018).contains(" 91\r\n"));
    CHECK(Written(first, release).contains(" 91\r\n        7\r\n"));
  }
}

TEST_CASE("vertex: its identifier written from R2010, whatever release it came from, never in R12")
{
  const DxfIO::Document file = Heavy();
  const Vertex &first = Traced(file.Entities, 0x48a).Vertices.Children.front();
  CHECK(Moved(first, Version::R2010, Version::R2004).contains(" 91\r\n        7\r\n"));
  CHECK_FALSE(Written(first, Version::R12).contains(" 91\r\n"));
}

TEST_CASE("vertex: a face's corners, negative for an edge not drawn")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x374);
  REQUIRE(taken.Vertices.Children.size() == 7);
  const Vertex &square = taken.Vertices.Children.at(5);
  CHECK(square.First == 1);
  CHECK(square.Fourth == 4);
  const Vertex &triangle = taken.Vertices.Children.at(6);
  CHECK(triangle.First == -2);
  CHECK(triangle.Third == 3);
  CHECK(triangle.Fourth == 0);
}

TEST_CASE("vertex: a face's corners of 0 left out")
{
  const DxfIO::Document file = Heavy();
  const Polyline &taken = Traced(file.Entities, 0x482);
  REQUIRE(taken.Vertices.Children.size() == 5);
  const std::string text = Written(taken.Vertices.Children.at(3));
  CHECK(text.contains(" 72\r\n     2\r\n"));
  CHECK_FALSE(text.contains(" 73\r\n"));
  CHECK_FALSE(text.contains(" 74\r\n"));
}

TEST_CASE("vertex: its markers from its own flags, a face's alone")
{
  constexpr std::array<std::pair<int, std::string_view>, 5> all{{
      {0, "100\r\nAcDbVertex\r\n100\r\nAcDb2dVertex\r\n"},
      {32, "100\r\nAcDbVertex\r\n100\r\nAcDb3dPolylineVertex\r\n"},
      {64, "100\r\nAcDbVertex\r\n100\r\nAcDbPolygonMeshVertex\r\n"},
      {192, "100\r\nAcDbVertex\r\n100\r\nAcDbPolyFaceMeshVertex\r\n"},
      {128, "100\r\nAcDbFaceRecord\r\n"},
  }};
  for (const auto &[flags, markers] : all)
  {
    CAPTURE(flags);
    const std::string text = Written(Flagged(flags));
    CHECK(text.contains(markers));
    CHECK(text.find("AcDbVertex") == text.rfind("AcDbVertex"));
    CHECK(DxfIO::Read<Vertex>(text).Flags == flags);
  }
}

TEST_CASE("vertex: a face keeps to AcDbFaceRecord, no AcDbVertex")
{
  CHECK_FALSE(Written(Flagged(128)).contains("AcDbVertex"));
}

TEST_CASE("vertex: a new one is its marker and its point, nothing else")
{
  CHECK(Written(Vertex{}).ends_with("100\r\nAcDb2dVertex\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"));
}

TEST_CASE("vertex: a width written at 0, unlike a width not given")
{
  Vertex held = Flagged(0);
  held.Start = 0.0;
  const std::string text = Written(held);
  CHECK(text.contains(" 40\r\n0.0\r\n"));
  CHECK_FALSE(text.contains(" 41\r\n"));
}

TEST_CASE("vertex: an old writer's AcDbVertex before a face's marker read")
{
  constexpr std::string_view text = "  0\r\nVERTEX\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
                                    "  8\r\n0\r\n100\r\nAcDbVertex\r\n100\r\nAcDbFaceRecord\r\n"
                                    " 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n 70\r\n   128\r\n"
                                    " 71\r\n     1\r\n";
  const Vertex taken = DxfIO::Read<Vertex>(text);
  CHECK(taken.Flags == 128);
  CHECK(taken.First == 1);
  CHECK_FALSE(Written(taken).contains("AcDbVertex"));
}
} // namespace DxfIO::Entities
