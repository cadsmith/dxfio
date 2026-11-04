#include "styles.hpp"

#include <string>
#include <utility>
#include <vector>

namespace DxfIO::Empty
{
namespace
{
constexpr double Texture = 0.0007999999797903;
constexpr int Channels = 63;
constexpr std::size_t Across = 5;
constexpr double Half = 0.5;
constexpr double Title = 6;
constexpr int Centered = 2;
constexpr int Middle = 5;

auto Named(std::string name, Handle self) -> Objects::Material
{
  Objects::Material out{};
  out.Base.Self = self;
  out.Name = std::move(name);
  out.Channels = Channels;
  return out;
}

auto Global(Handle self) -> Objects::Material
{
  Objects::Material out = Named("Global", self);
  DxfIO::Matrix placed{};
  placed.Cells.front() = Texture;
  placed.Cells.at(Across) = Texture;
  out.Maps->Diffuse.Transform = placed;
  out.Maps->Reflection.Transform = placed;
  out.Maps->Opacity.Transform = placed;
  out.Maps->Bump.Transform = placed;
  return out;
}

auto Line(double offset) -> Objects::Mline::Element
{
  Objects::Mline::Element out{};
  out.Offset = offset;
  return out;
}

auto Cell(double height, int alignment) -> Objects::Table::Cell
{
  Objects::Table::Cell out{};
  out.Style = "Standard";
  out.Height = height;
  out.Alignment = alignment;
  return out;
}
} // namespace

auto Materials(const Roots &ids) -> std::vector<Objects::Material>
{
  std::vector<Objects::Material> out;
  out.push_back(Named("ByBlock", ids.Materials.Byblock));
  out.push_back(Named("ByLayer", ids.Materials.Bylayer));
  out.push_back(Global(ids.Materials.Global));
  return out;
}

auto Mleaderstyle(const Roots &ids, Seed &handles) -> Objects::MLeaderStyle
{
  Objects::MLeaderStyle out{};
  out.Base.Self = handles.Take();
  out.Description = "Standard";
  out.Leader->Linetype = ids.Linetype;
  out.Text->Style = ids.Style;
  return out;
}

auto Mlinestyle(Seed &handles) -> Objects::MLineStyle
{
  Objects::MLineStyle out{};
  out.Base.Self = handles.Take();
  out.Name = "STANDARD";
  out.Elements = {Line(Half), Line(-Half)};
  return out;
}

auto Tablestyle(Seed &handles) -> Objects::TableStyle
{
  Objects::TableStyle out{};
  out.Base.Self = handles.Take();
  out.Description = "Standard";
  out.Cells = {
      Cell(Objects::Table::Height, Centered),
      Cell(Title, Middle),
      Cell(Objects::Table::Height, Middle),
  };
  return out;
}
} // namespace DxfIO::Empty
