#include "empty/classes.hpp"

#include "checks.hpp"
#include "entities/io.hpp"
#include "header/io.hpp"
#include "objects/io.hpp"
#include "walk.hpp"

#include "dxfio/raw.hpp"

#include <algorithm>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace DxfIO::Audit
{
namespace
{
// A kind the drawing holds, and how many of it.
struct Counted
{
  std::string_view Name;
  std::int64_t Count{};
};

void Count(std::vector<Counted> &all, std::string_view name)
{
  const auto found = std::ranges::find(all, name, &Counted::Name);
  if (found == all.end())
  {
    all.push_back(Counted{.Name = name, .Count = 1});
    return;
  }
  ++found->Count;
}

// The kinds of the drawing's entities and objects, in the order they are first met.
auto Kinds(const Document &drawing) -> std::vector<Counted>
{
  std::vector<Counted> out;
  Lists(drawing,
        [&out](const auto &all) -> void
        {
          for (const Entity &one : all)
          {
            Count(out, Entities::Named(one));
          }
        });
  for (const Object &one : drawing.Objects)
  {
    Count(out, Objects::Type(one));
  }
  return out;
}

// Whether a CLASS record declares the kind of that name in its group 1.
auto Declares(const Raw &one, std::string_view name) -> bool
{
  const auto named = std::ranges::find(one.Groups, 1, &Group::Code);
  const auto *held = named == one.Groups.end() ? nullptr : std::get_if<std::string>(&named->Value);
  return held != nullptr && *held == name;
}
} // namespace

// AutoCAD declares every kind of its own that a drawing holds in a CLASS record, and readers that need it misread
// a file without one: a kind present with no CLASS is an error, and given the record AutoCAD writes when fixed.
template <class DocumentT>
void Classed(Auditor<DocumentT> &with)
{
  const Version release = Header::Declared(with.Drawing->Header);
  if (release < Version::R13)
  {
    return;
  }
  for (const Counted &kind : Kinds(*with.Drawing))
  {
    const Empty::Class *known = Empty::Known(kind.Name, release);
    const auto declares = [&kind](const Raw &one) -> bool
    {
      return Declares(one, kind.Name);
    };
    if (known == nullptr || std::ranges::any_of(with.Drawing->Classes, declares))
    {
      continue;
    }
    with.Note(Severity::Error, std::format("no CLASS for {}", kind.Name), {});
    if constexpr (Auditor<DocumentT>::Fix)
    {
      with.Drawing->Classes.push_back(Empty::Declared(*known, release, kind.Count));
    }
  }
}

template void Classed(Auditor<Document> &with);

template void Classed(Auditor<const Document> &with);
} // namespace DxfIO::Audit
