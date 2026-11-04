#include "objects.hpp"

#include "layouts.hpp"
#include "scales.hpp"
#include "styles.hpp"
#include "unknown/io.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>

namespace DxfIO::Empty
{
namespace
{
constexpr std::size_t Row = 10;

constexpr std::string_view Letters = "AB";

struct Shelf
{
  std::string Name;
  Objects::Dictionary Dictionary{};
  std::vector<Object> Held;
  Handle Fallback{};
};

void Entered(Objects::Dictionary &into, std::string name, DxfIO::Base &one)
{
  one.Owner = into.Base.Self;
  one.Reactors = {into.Base.Self};
  into.Entries.push_back(Objects::Entry{.Name = std::move(name), .Soft = one.Self, .Hard = {}});
}

auto Opened(std::string name, Seed &handles) -> Shelf
{
  Shelf out{};
  out.Name = std::move(name);
  out.Dictionary.Base.Self = handles.Take();
  return out;
}

template <class KindT>
void Kept(Shelf &into, std::string name, KindT one)
{
  Entered(into.Dictionary, std::move(name), one.Base);
  into.Held.emplace_back(std::move(one));
}

template <class KindT>
auto Single(std::string name, KindT one, Seed &handles) -> Shelf
{
  Shelf out = Opened(std::move(name), handles);
  Kept(out, "Standard", std::move(one));
  return out;
}

auto Layouts(const Roots &ids, Handle viewport, Seed &handles) -> Shelf
{
  Shelf out = Opened("ACAD_LAYOUT", handles);
  for (Objects::Layout &one : Empty::Layouts(ids, viewport))
  {
    std::string name = one.Name;
    Kept(out, std::move(name), std::move(one));
  }
  return out;
}

auto Materials(const Roots &ids, Seed &handles) -> Shelf
{
  Shelf out = Opened("ACAD_MATERIAL", handles);
  for (Objects::Material &one : Empty::Materials(ids))
  {
    std::string name = one.Name;
    Kept(out, std::move(name), std::move(one));
  }
  return out;
}

auto Plotstyles(const Roots &ids, Seed &handles) -> Shelf
{
  Shelf out = Opened("ACAD_PLOTSTYLENAME", handles);
  Objects::Placeholder normal{};
  normal.Base.Self = ids.Normal;
  Kept(out, "Normal", std::move(normal));
  out.Fallback = ids.Normal;
  return out;
}

auto Scales(Seed &handles) -> Shelf
{
  Shelf out = Opened("ACAD_SCALELIST", handles);
  std::size_t at = 0;
  for (Objects::Scale &one : Empty::Scales(handles))
  {
    Kept(out, std::format("{}{}", Letters.at(at / Row), at % Row), std::move(one));
    ++at;
  }
  return out;
}

auto Shelves(const Roots &ids, Handle viewport, Seed &handles) -> std::vector<Shelf>
{
  std::vector<Shelf> out;
  out.push_back(Opened("ACAD_GROUP", handles));
  out.push_back(Layouts(ids, viewport, handles));
  out.push_back(Materials(ids, handles));
  out.push_back(Single("ACAD_MLEADERSTYLE", Mleaderstyle(ids, handles), handles));
  out.push_back(Single("ACAD_MLINESTYLE", Mlinestyle(handles), handles));
  out.push_back(Opened("ACAD_PLOTSETTINGS", handles));
  out.push_back(Plotstyles(ids, handles));
  out.push_back(Scales(handles));
  out.push_back(Single("ACAD_TABLESTYLE", Tablestyle(handles), handles));
  return out;
}

auto Shelved(Shelf &one) -> Object
{
  if (!one.Fallback)
  {
    return std::move(one.Dictionary);
  }
  return Objects::DictionaryWithDefault{
      .Dictionary = std::move(one.Dictionary),
      .Fallback = one.Fallback,
  };
}

auto Rooted(const Roots &ids, std::vector<Shelf> all) -> std::vector<Object>
{
  Objects::Dictionary root{};
  root.Base.Self = ids.Dictionary;
  for (Shelf &one : all)
  {
    Entered(root, one.Name, one.Dictionary.Base);
  }
  std::vector<Object> out;
  out.emplace_back(std::move(root));
  for (Shelf &one : all)
  {
    out.push_back(Shelved(one));
  }
  for (Shelf &one : all)
  {
    std::ranges::move(one.Held, std::back_inserter(out));
  }
  return out;
}
} // namespace

auto Objects(const Roots &ids, Handle viewport, Seed &handles) -> std::vector<Object>
{
  return Rooted(ids, Shelves(ids, viewport, handles));
}

auto Early(const Roots &ids, Seed &handles) -> std::vector<Object>
{
  std::vector<Shelf> all;
  all.push_back(Opened("ACAD_GROUP", handles));
  all.push_back(Single("ACAD_MLINESTYLE", Mlinestyle(handles), handles));
  return Rooted(ids, std::move(all));
}
} // namespace DxfIO::Empty
