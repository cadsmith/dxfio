#include "roots.hpp"

namespace DxfIO::Empty
{
  auto Taken(Seed &handles) -> Roots
  {
    Roots out{};
    out.Dictionary        = handles.Take();
    out.Records           = Spaces{.Model = handles.Take(), .Paper = handles.Take()};
    out.Layouts           = Spaces{.Model = handles.Take(), .Paper = handles.Take()};
    out.Materials.Byblock = handles.Take();
    out.Materials.Bylayer = handles.Take();
    out.Materials.Global  = handles.Take();
    out.Normal            = handles.Take();
    out.Linetype          = handles.Take();
    out.Style             = handles.Take();
    return out;
  }
} // namespace DxfIO::Empty
