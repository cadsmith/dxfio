#include "dxfio/save.hpp"

#include "blocks/io.hpp"
#include "chained.hpp"
#include "down/down.hpp"
#include "entities/io.hpp"
#include "fields/owned.hpp"
#include "fields/text.hpp"
#include "header/io.hpp"
#include "objects/io.hpp"
#include "plan/foreign.hpp"
#include "raw/io.hpp"
#include "tables/io.hpp"
#include "tags/sentinel.hpp"
#include "tags/writer.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <fstream>
#include <functional>
#include <ios>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace DxfIO
{
namespace
{
using Writing = std::expected<void, Failure> (*)(Fields::Writing &to, const Document &from, std::string_view name);

struct Known
{
  std::string_view Name;
  Writing Write{};
};

constexpr std::array Knowns{
    Known{
        .Name = "HEADER",
        .Write = [](Fields::Writing &to, const Document &from, std::string_view) -> std::expected<void, Failure>
        { return Header::Write(to, from.Header); },
    },
    Known{
        .Name = "CLASSES",
        .Write = [](Fields::Writing &to, const Document &from, std::string_view) -> std::expected<void, Failure>
        { return Put(to, from.Classes); },
    },
    Known{
        .Name = "TABLES",
        .Write = [](Fields::Writing &to, const Document &from, std::string_view) -> std::expected<void, Failure>
        { return Tables::Write(to, from); },
    },
    Known{
        .Name = "BLOCKS",
        .Write = [](Fields::Writing &to, const Document &from, std::string_view) -> std::expected<void, Failure>
        { return Blocks::Write(to, from.Blocks); },
    },
    Known{
        .Name = "ENTITIES",
        .Write = [](Fields::Writing &to, const Document &from, std::string_view) -> std::expected<void, Failure>
        { return Entities::Write(to, from.Entities); },
    },
    Known{
        .Name = "OBJECTS",
        .Write = [](Fields::Writing &to, const Document &from, std::string_view) -> std::expected<void, Failure>
        { return Objects::Write(to, from.Objects); },
    },
};

auto Foreign(Fields::Writing &to, const Document &from, std::string_view name) -> std::expected<void, Failure>
{
  const auto found = std::ranges::find(from.Sections, name, &Section::Name);
  if (found == from.Sections.end())
  {
    return {};
  }
  return Chained(found->Groups, [&](const Group &one) -> std::expected<void, Failure> { return Fields::Put(to, one); });
}

auto Absent(std::string_view name, Version release) -> bool
{
  return name == "ACDSDATA" && release < Version::R2013;
}

// The sections DXF defines, in the order AutoCAD writes them.
constexpr std::array<std::string_view, 6> Defined{"HEADER", "CLASSES", "TABLES", "BLOCKS", "ENTITIES", "OBJECTS"};

// Whether a table is written: named, or holding records that need it.
template <class RecordT>
auto Filled(const Tables::Table<RecordT> &one) -> bool
{
  return one.Head.Name.size() + one.Records.size() != 0;
}

auto Tabled(const Document &drawing) -> bool
{
  const std::array filled{
      Filled(drawing.Vport),       Filled(drawing.Ltype),  Filled(drawing.Layer), Filled(drawing.Style),
      Filled(drawing.View),        Filled(drawing.Ucs),    Filled(drawing.Appid), Filled(drawing.Dimstyle),
      Filled(drawing.BlockRecord), !drawing.Loose.empty(),
  };
  return std::ranges::any_of(filled, std::identity{});
}

// Whether a drawing holds something for the section of that name, so that the file needs it.
auto Holds(const Document &drawing, std::string_view name) -> bool
{
  if (name == "TABLES")
  {
    return Tabled(drawing);
  }
  if (name == "HEADER")
  {
    return !drawing.Header.empty();
  }
  if (name == "CLASSES")
  {
    return !drawing.Classes.empty();
  }
  if (name == "BLOCKS")
  {
    return !drawing.Blocks.empty();
  }
  return name == "ENTITIES" ? !drawing.Entities.empty() : !drawing.Objects.empty();
}

// Where a section stands among those DXF defines; past them all for one it does not.
auto Rank(std::string_view name) -> std::size_t
{
  return static_cast<std::size_t>(std::ranges::find(Defined, name) - Defined.begin());
}

// The sections to write: those the order names, as it names them, and after that any other the drawing holds
// something for, each where DXF puts it.
auto Planned(const Document &drawing) -> std::vector<std::string>
{
  std::vector<std::string> out = drawing.Order;
  for (const std::string_view name : Defined)
  {
    if (std::ranges::contains(out, name) || !Holds(drawing, name))
    {
      continue;
    }
    const auto later =
        std::ranges::find_if(out, [name](const std::string &one) -> bool { return Rank(one) > Rank(name); });
    out.emplace(later, name);
  }
  for (const Section &one : drawing.Sections)
  {
    if (!std::ranges::contains(out, one.Name))
    {
      out.push_back(one.Name);
    }
  }
  return out;
}

auto Pick(std::string_view name) -> Writing
{
  const auto *found = std::ranges::find(Knowns, name, &Known::Name);
  return found == Knowns.end() ? Foreign : found->Write;
}

auto Named(Fields::Writing &to, std::string_view text) -> std::expected<void, Failure>
{
  return Fields::Word(to, 0, text);
}

auto Text(const Document &drawing, const Options &with) -> Encoding::Scheme
{
  const Encoding::Codepage *asked = Encoding::Find(with.Codepage);
  if (asked != nullptr)
  {
    return Encoding::Of(with.Release, *asked);
  }
  return with.Release < Version::R2007 && Header::Opaque(drawing.Header)
             ? Encoding::Opaque()
             : Encoding::Of(with.Release, Header::Named(drawing.Header));
}

auto Form(const Options &with) -> Tags::Form
{
  if (!with.Binary)
  {
    return Tags::Form::Ascii;
  }
  return with.Release < Version::R13 ? Tags::Form::Narrow : Tags::Form::Wide;
}

auto Written(const Document &drawing, const Options &with) -> std::expected<std::string, Failure>
{
  std::string out;
  if (with.Binary)
  {
    out.append(Tags::Sentinel());
  }
  Tags::Writer writer{out, Form(with)};
  Fields::Writing to{
      .To = &writer,
      .Release = with.Release,
      .Text = Text(drawing, with),
      .From = Header::Declared(drawing.Header),
  };
  return Chained(Planned(drawing),
                 [&](const std::string &name) -> std::expected<void, Failure>
                 {
                   if (Absent(name, to.Release))
                   {
                     return std::expected<void, Failure>{};
                   }
                   return Named(to, "SECTION")
                       .and_then([&] -> std::expected<void, Failure> { return Fields::Word(to, 2, name); })
                       .and_then([&] -> std::expected<void, Failure> { return Pick(name)(to, drawing, name); })
                       .and_then([&] -> std::expected<void, Failure> { return Named(to, "ENDSEC"); });
                 })
      .and_then([&to] -> std::expected<void, Failure> { return Named(to, "EOF"); })
      .transform([&out] -> std::string { return std::move(out); });
}

auto Moved(const Document &drawing, Version release, std::vector<Issue> *issues) -> Document
{
  Document out = Down::Narrowed(drawing, release, issues);
  if (release > Header::Declared(out.Header))
  {
    out.Objects = Plan::Foreign(std::move(out.Objects));
  }
  return out;
}
} // namespace

auto Save(const Document &drawing, const Options &with) -> std::expected<Saved, Failure>
{
  if (!with.Codepage.empty() && Encoding::Find(with.Codepage) == nullptr)
  {
    return std::unexpected{Failure{.Line = 0, .What = std::format("no code page named {}", with.Codepage)}};
  }
  Saved out;
  const bool same = with.Release == Header::Declared(drawing.Header);
  auto bytes = same ? Written(drawing, with) : Written(Moved(drawing, with.Release, &out.Issues), with);
  if (!bytes)
  {
    return std::unexpected{bytes.error()};
  }
  out.Bytes = std::move(*bytes);
  return out;
}

auto Save(const Document &drawing, std::ostream &to, const Options &with) -> std::expected<std::vector<Issue>, Failure>
{
  return Save(drawing, with)
      .and_then(
          [&to](Saved saved) -> std::expected<std::vector<Issue>, Failure>
          {
            to.write(saved.Bytes.data(), static_cast<std::streamsize>(saved.Bytes.size())).flush();
            if (!to)
            {
              return std::unexpected{Failure{.Line = 0, .What = "the stream would not take the drawing"}};
            }
            return std::move(saved.Issues);
          });
}

auto SaveFile(const Document &drawing, const std::filesystem::path &file, const Options &with)
    -> std::expected<std::vector<Issue>, Failure>
{
  std::ofstream to{file, std::ios::binary | std::ios::trunc};
  if (!to)
  {
    return std::unexpected{Failure{.Line = 0, .What = std::format("{} cannot be written", file.string())}};
  }
  return Save(drawing, to, with);
}
} // namespace DxfIO
