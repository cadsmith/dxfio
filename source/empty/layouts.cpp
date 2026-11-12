#include "layouts.hpp"

#include "empty/space.hpp"

#include <string>
#include <utility>
#include <vector>

namespace DxfIO::Empty
{
  namespace
  {
    constexpr std::string_view None = "none_device";

    auto Sheet() -> Objects::Plot
    {
      const Objects::Plot out{.Name   = "",
                              .Device = std::string{None},
                              .Size   = "ISO_A4_(210.00_x_297.00_MM)",
                              .View   = "",
                              .Left   = 7.5,
                              .Bottom = 20,
                              .Right  = 7.5,
                              .Top    =  20,
                              .Width  = 210,
                              .Height = 297,
                              .Origin =
                                  Vector{.X = 11.54999923706054, .Y = -13.65000009536743, .Z = 0},
                              .Lower      = Vector{},
                              .Upper      = Vector{},
                              .Paper      = 1,
                              .Drawing    = 8.704084754739808,
                              .Flags      = 11952,
                              .Units      =     1,
                              .Rotation   =     0,
                              .Type       =     0,
                              .Style      = "",
                              .Standard   = 0,
                              .Factor     = 0.1148885871608098,
                              .Shading    = 0,
                              .Resolution = Objects::Normal,
                              .Dots       = Objects::Dpi,
                              .Image      = Vector{}};
      return out;
    }
  } // namespace

  auto Layout(std::string name, Handle self, Handle record) -> Objects::Layout
  {
    Objects::Layout out{};
    out.Base.Self    = self;
    out.Plot->Device = std::string{None};
    out.Name         = std::move(name);
    out.Block        = record;
    return out;
  }

  auto Layouts(const Roots &ids, Handle viewport) -> std::vector<Objects::Layout>
  {
    Objects::Layout paper = Layout("Layout1", ids.Layouts.Paper, ids.Records.Paper);
    paper.Order           = 1;
    Objects::Layout model = Layout("Model", ids.Layouts.Model, ids.Records.Model);
    *model.Plot           = Sheet();
    model.Viewport        = viewport;
    std::vector<Objects::Layout> out;
    out.push_back(std::move(paper));
    out.push_back(std::move(model));
    return out;
  }
} // namespace DxfIO::Empty
