#include "dxfio/load.hpp"

#include "blocks/io.hpp"
#include "chained.hpp"
#include "entities/io.hpp"
#include "fields/owned.hpp"
#include "file/file.hpp"
#include "file/sections/header.hpp"
#include "file/sections/nests.hpp"
#include "file/sections/records.hpp"
#include "header/io.hpp"
#include "objects/io.hpp"
#include "past.hpp"
#include "raw/io.hpp"
#include "tables/io.hpp"
#include "tags/codes.hpp"
#include "tags/tags.hpp"

#include "dxfio/version.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <istream>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace DxfIO
{
namespace
{
using Filled = std::expected<void, Failure>;

using Filling = Filled (*)(const File::Section &from, Loaded &into, const Fields::Reading &with);

struct Known
{
  std::string_view Name;
  Filling Fill{};
};

constexpr std::array Knowns{
    Known{
        .Name = "HEADER",
        .Fill = [](const File::Section &from, Loaded &into, const Fields::Reading &with) -> Filled
        {
          return File::Sections::Header(from.Body).transform(
              [&into, &with](const std::vector<File::Variable> &parts) -> void
              { into.Document.Header = Header::Read(parts, with); });
        },
    },
    Known{
        .Name = "CLASSES",
        .Fill = [](const File::Section &from, Loaded &into, const Fields::Reading &with) -> Filled
        {
          return File::Sections::Records(from.Body).transform(
              [&into, &with](const std::vector<File::Record> &parts) -> void
              { into.Document.Classes = Copied(parts, with); });
        },
    },
    Known{
        .Name = "TABLES",
        .Fill = [](const File::Section &from, Loaded &into, const Fields::Reading &with) -> Filled
        {
          return File::Sections::Tables(from.Body).transform(
              [&into, &with](const std::vector<File::Nest> &parts) -> void
              { Tables::Read(parts, into.Document, with); });
        },
    },
    Known{
        .Name = "BLOCKS",
        .Fill = [](const File::Section &from, Loaded &into, const Fields::Reading &with) -> Filled
        {
          return File::Sections::Blocks(from.Body).transform(
              [&into, &with](const std::vector<File::Nest> &parts) -> void
              { into.Document.Blocks = Blocks::Read(parts, with); });
        },
    },
    Known{
        .Name = "ENTITIES",
        .Fill = [](const File::Section &from, Loaded &into, const Fields::Reading &with) -> Filled
        {
          return File::Sections::Records(from.Body).transform(
              [&into, &with](const std::vector<File::Record> &parts) -> void
              { into.Document.Entities = Entities::Read(parts, with); });
        },
    },
    Known{
        .Name = "OBJECTS",
        .Fill = [](const File::Section &from, Loaded &into, const Fields::Reading &with) -> Filled
        {
          return File::Sections::Records(from.Body).transform(
              [&into, &with](const std::vector<File::Record> &parts) -> void
              { into.Document.Objects = Objects::Read(parts, with); });
        },
    },
};

auto Foreign(const File::Section &from, Loaded &into, const Fields::Reading &with) -> Filled
{
  Section held{.Name = std::string{from.Name}, .Groups = {}};
  held.Groups.reserve(from.Body.size());
  for (const Tags::Tag &one : from.Body)
  {
    held.Groups.push_back(Fields::Owned(one, with.Text));
  }
  into.Document.Sections.push_back(std::move(held));
  return {};
}

auto Filler(std::string_view name) -> Filling
{
  const auto *found = std::ranges::find(Knowns, name, &Known::Name);
  return found == Knowns.end() ? Foreign : found->Fill;
}

auto Variables(std::span<const File::Section> all) -> std::expected<std::vector<File::Variable>, Failure>
{
  const auto found = std::ranges::find(all, "HEADER", &File::Section::Name);
  if (found == all.end())
  {
    return std::vector<File::Variable>{};
  }
  return File::Sections::Header(found->Body);
}

auto Taken(const File::Section &one, Loaded &into, const Fields::Reading &with) -> Filled
{
  const std::string name{one.Name};
  if (std::ranges::contains(into.Document.Order, name))
  {
    into.Issues.push_back(Issue{
        .Line = 0,
        .What = std::format("section {} given again, the first kept", name),
    });
    return {};
  }
  into.Document.Order.push_back(name);
  return Filler(one.Name)(one, into, with);
}

auto Seeded(Document &drawing) -> Handle *
{
  const auto found = std::ranges::find(drawing.Header, "$HANDSEED", &Header::Variable::Name);
  if (found == drawing.Header.end() || found->Values.empty())
  {
    return nullptr;
  }
  return std::get_if<Handle>(&found->Values.front().Value);
}

constexpr std::string_view Named = "$ACADVER";

// Whether a file with no $ACADVER was written for R13 or later: a record of its carries a subclass marker, which
// no earlier release has.
auto Marked(std::span<const File::Section> all) -> bool
{
  return std::ranges::any_of(all, [](const File::Section &one) -> bool
                             { return std::ranges::contains(one.Body, Tags::Codes::Marker, &Tags::Tag::Code); });
}

// Whether a file names no $ACADVER but its records carry subclass markers, so it is read as R2000, not R12.
auto Inferred(std::span<const File::Variable> head, std::span<const File::Section> all) -> bool
{
  return !std::ranges::contains(head, Named, &File::Variable::Name) && Marked(all);
}

// Gives a drawing read as R2000 for its markers the $ACADVER it lacked, first in its header.
void Dated(Loaded &into)
{
  into.Issues.push_back(Issue{
      .Line = 0,
      .What = "no $ACADVER, read as R2000 for the subclass markers its records carry",
  });
  into.Document.Header.insert(into.Document.Header.begin(),
                              Header::Variable{
                                  .Name = std::string{Named},
                                  .Values = {Group{.Code = 1, .Value = std::string{Acadver(Version::R2000)}}},
                              });
}

void Raised(Loaded &into)
{
  Handle *seed = Seeded(into.Document);
  if (seed == nullptr || !*seed)
  {
    return;
  }
  const Handle past = Past(into.Document);
  if (past <= *seed)
  {
    return;
  }
  into.Issues.push_back(Issue{
      .Line = 0,
      .What = std::format("$HANDSEED {} is not past the handles in use, raised to {}", Hex(*seed), Hex(past)),
  });
  *seed = past;
}
} // namespace

auto Load(std::string_view bytes) -> std::expected<Loaded, Failure>
{
  Loaded out;
  const auto all = Tags::All(bytes, &out.Issues);
  if (!all)
  {
    return std::unexpected{all.error()};
  }
  const auto sections = File::Read(*all, &out.Issues);
  if (!sections)
  {
    return std::unexpected{sections.error()};
  }
  const auto head = Variables(*sections);
  if (!head)
  {
    return std::unexpected{head.error()};
  }
  const bool inferred = Inferred(*head, *sections);
  const Version release = inferred ? Version::R2000 : Header::Declared(*head);
  const bool opaque = Header::Opaque(*head);
  const Fields::Reading with{
      .Release = release,
      .Text = opaque ? Encoding::Opaque() : Encoding::Of(release, Header::Named(*head)),
      .Issues = &out.Issues,
  };
  if (opaque)
  {
    out.Issues.push_back(Issue{
        .Line = 0,
        .What = "the code page is not carried, text kept as bytes",
    });
  }
  const Filled filled =
      Chained(*sections, [&out, &with](const File::Section &one) -> Filled { return Taken(one, out, with); });
  if (!filled)
  {
    return std::unexpected{filled.error()};
  }
  if (inferred)
  {
    Dated(out);
  }
  Raised(out);
  return out;
}

auto Load(std::istream &from) -> std::expected<Loaded, Failure>
{
  if (from.fail())
  {
    return std::unexpected{Failure{.Line = 0, .What = "the stream could not be read"}};
  }
  const std::string bytes{std::istreambuf_iterator<char>{from}, std::istreambuf_iterator<char>{}};
  return Load(bytes);
}

auto LoadFile(const std::filesystem::path &file) -> std::expected<Loaded, Failure>
{
  std::ifstream from{file, std::ios::binary};
  if (!from)
  {
    return std::unexpected{Failure{.Line = 0, .What = std::format("{} cannot be read", file.string())}};
  }
  std::error_code unsized;
  const std::uintmax_t size = std::filesystem::file_size(file, unsized);
  if (unsized)
  {
    return Load(from);
  }
  std::string bytes;
  bytes.resize_and_overwrite(static_cast<std::size_t>(size),
                             [&from](char *into, std::size_t room) -> std::size_t
                             {
                               from.read(into, static_cast<std::streamsize>(room));
                               return static_cast<std::size_t>(from.gcount());
                             });
  bytes.append(std::istreambuf_iterator<char>{from}, std::istreambuf_iterator<char>{});
  return Load(bytes);
}
} // namespace DxfIO
