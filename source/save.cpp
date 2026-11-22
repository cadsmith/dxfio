#include "dxfio/save.hpp"

#include "blocks/io.hpp"
#include "chained.hpp"
#include "down/down.hpp"
#include "entities/io.hpp"
#include "fields/owned.hpp"
#include "fields/text.hpp"
#include "header/io.hpp"
#include "lift/lift.hpp"
#include "objects/io.hpp"
#include "parallel.hpp"
#include "plan/foreign.hpp"
#include "raw/io.hpp"
#include "seek.hpp"
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
    using Writing = std::expected<void, Failure> (*)(Fields::Writing &to, const Drawing &from,
                                                     std::string_view name);

    struct Known
    {
      std::string_view Name;
      Writing          Write{};
    };

    constexpr std::array Knowns{
        Known{
            .Name  = "HEADER",
            .Write = [](Fields::Writing &to, const Drawing &from,
                        std::string_view) -> std::expected<void, Failure>
            { return Header::Write(to, from.Header); },
        },
        Known{
            .Name  = "CLASSES",
            .Write = [](Fields::Writing &to, const Drawing &from,
                        std::string_view) -> std::expected<void, Failure>
            { return Put(to, from.Classes); },
        },
        Known{
            .Name  = "TABLES",
            .Write = [](Fields::Writing &to, const Drawing &from,
                        std::string_view) -> std::expected<void, Failure>
            { return Tables::Write(to, from); },
        },
        Known{
            .Name  = "BLOCKS",
            .Write = [](Fields::Writing &to, const Drawing &from,
                        std::string_view) -> std::expected<void, Failure>
            { return Blocks::Write(to, from.Blocks); },
        },
        Known{
            .Name  = "ENTITIES",
            .Write = [](Fields::Writing &to, const Drawing &from,
                        std::string_view) -> std::expected<void, Failure>
            { return Entities::Write(to, from.Entities); },
        },
        Known{
            .Name  = "OBJECTS",
            .Write = [](Fields::Writing &to, const Drawing &from,
                        std::string_view) -> std::expected<void, Failure>
            { return Objects::Write(to, from.Objects); },
        },
    };

    auto Foreign(Fields::Writing &to, const Drawing &from, std::string_view name)
        -> std::expected<void, Failure>
    {
      const auto found = std::ranges::find(from.Sections, name, &Section::Name);
      if (found == from.Sections.end())
      {
        return {};
      }
      return Chained(found->Tags, [&](const Tag &one) -> std::expected<void, Failure>
                     { return Fields::Put(to, one); });
    }

    auto Absent(std::string_view name, Version release) -> bool
    {
      return name == "ACDSDATA" && release < Version::R2013;
    }

    /**
     *  The sections DXF defines, in the order AutoCAD writes them.
     */
    constexpr std::array<std::string_view, 6> Defined{"HEADER", "CLASSES",  "TABLES",
                                                      "BLOCKS", "ENTITIES", "OBJECTS"};

    /**
     *  Whether a table is written: named, or holding records that need it.
     */
    template <class RecordT>
    auto Filled(const Tables::Table<RecordT> &one) -> bool
    {
      return one.Head.Name.size() + one.Records.size() != 0;
    }

    auto Tabled(const Drawing &drawing) -> bool
    {
      const std::array filled{
          Filled(drawing.VPort),  Filled(drawing.LType),    Filled(drawing.Layer),
          Filled(drawing.Style),  Filled(drawing.View),     Filled(drawing.Ucs),
          Filled(drawing.AppId),  Filled(drawing.DimStyle), Filled(drawing.BlockRecord),
          !drawing.Loose.empty(),
      };
      return std::ranges::any_of(filled, std::identity{});
    }

    /**
     *  Whether a drawing holds something for the section of that name, so that the file needs it.
     */
    auto Holds(const Drawing &drawing, std::string_view name) -> bool
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

    /**
     *  Where a section stands among those DXF defines; past them all for one it does not.
     */
    auto Rank(std::string_view name) -> std::size_t
    {
      return static_cast<std::size_t>(std::ranges::find(Defined, name) - Defined.begin());
    }

    /**
     *  The sections to write: those the order names, as it names them, and after that any other the
     *  drawing holds something for, each where DXF puts it.
     */
    auto Planned(const Drawing &drawing) -> std::vector<std::string>
    {
      std::vector<std::string> out = drawing.Order;
      for (const std::string_view name : Defined)
      {
        if (std::ranges::contains(out, name) || !Holds(drawing, name))
        {
          continue;
        }
        const auto later = std::ranges::find_if(out, [name](const std::string &one) -> bool
                                                { return Rank(one) > Rank(name); });
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
      const auto *found = Seek(Knowns, name, &Known::Name);
      return found == nullptr ? Foreign : found->Write;
    }

    auto Named(Fields::Writing &to, std::string_view text) -> std::expected<void, Failure>
    {
      return Fields::Word(to, 0, text);
    }

    auto Text(const Drawing &drawing, const SaveOptions &with) -> Encoding::Scheme
    {
      const Encoding::Codepage *asked  = Encoding::Find(with.Codepage);
      const bool                opaque = Header::Opaque(drawing.Header);
      if (opaque && asked == nullptr && with.Release < Version::R2007)
      {
        return Encoding::Opaque();
      }
      const Encoding::Codepage &page = asked != nullptr ? *asked : Header::Named(drawing.Header);
      return opaque ? Encoding::Opened(with.Release, page) : Encoding::Of(with.Release, page);
    }

    auto Form(const SaveOptions &with) -> Tags::Form
    {
      if (!with.Binary)
      {
        return Tags::Form::Ascii;
      }
      return with.Release < Version::R13 ? Tags::Form::Narrow : Tags::Form::Wide;
    }

    auto Written(const Drawing &drawing, const SaveOptions &with)
        -> std::expected<std::string, Failure>
    {
      std::string out;
      if (with.Binary)
      {
        out.append(Tags::Sentinel());
      }
      Tags::Writer    writer{out, Form(with)};
      Fields::Writing to{
          .To      = &writer,
          .Release = with.Release,
          .Text    = Text(drawing, with),
          .From    = Header::Declared(drawing.Header),
          .Threads = Threads(with.Threads),
      };
      return Chained(Planned(drawing),
                     [&](const std::string &name) -> std::expected<void, Failure>
                     {
                       if (Absent(name, to.Release))
                       {
                         return std::expected<void, Failure>{};
                       }
                       return Named(to, "SECTION")
                           .and_then([&] -> std::expected<void, Failure>
                                     { return Fields::Word(to, 2, name); })
                           .and_then([&] -> std::expected<void, Failure>
                                     { return Pick(name)(to, drawing, name); })
                           .and_then([&] -> std::expected<void, Failure>
                                     { return Named(to, "ENDSEC"); });
                     })
          .and_then([&to] -> std::expected<void, Failure> { return Named(to, "EOF"); })
          .transform([&out] -> std::string { return std::move(out); });
    }

    auto Moved(const Drawing &drawing, Version release, std::vector<Issue> *issues) -> Drawing
    {
      if (release >= Version::R13 && Header::Declared(drawing.Header) < Version::R13)
      {
        return Lift::Lifted(drawing, release);
      }
      Drawing out = Down::Narrowed(drawing, release, issues);
      if (release > Header::Declared(out.Header))
      {
        out.Objects = Plan::Foreign(std::move(out.Objects));
      }
      return out;
    }

    /**
     *  Writes the bytes of a save to a stream, which then holds them all or failed.
     */
    auto Streamed(Saved saved, std::ostream &to) -> std::expected<std::vector<Issue>, Failure>
    {
      to.write(saved.Bytes.data(), static_cast<std::streamsize>(saved.Bytes.size())).flush();
      if (!to)
      {
        return std::unexpected{Failure{.Code = FailureCode::Io, .What = "stream not writable"}};
      }
      return std::move(saved.Issues);
    }
  } // namespace

  auto Save(const Drawing &drawing, const SaveOptions &with) -> std::expected<Saved, Failure>
  {
    if (!with.Codepage.empty() && Encoding::Find(with.Codepage) == nullptr)
    {
      return std::unexpected{Failure{.Code = FailureCode::Refused,
                                     .What = std::format("no code page named {}", with.Codepage)}};
    }
    Saved      out;
    const bool same = with.Release == Header::Declared(drawing.Header);
    auto       bytes =
        same ? Written(drawing, with) : Written(Moved(drawing, with.Release, &out.Issues), with);
    if (!bytes)
    {
      return std::unexpected{bytes.error()};
    }
    out.Bytes = std::move(*bytes);
    return out;
  }

  auto Save(const Drawing &drawing, std::ostream &to, const SaveOptions &with)
      -> std::expected<std::vector<Issue>, Failure>
  {
    return Save(drawing, with)
        .and_then([&to](Saved saved) -> std::expected<std::vector<Issue>, Failure>
                  { return Streamed(std::move(saved), to); });
  }

  auto SaveFile(const Drawing &drawing, const std::filesystem::path &file, const SaveOptions &with)
      -> std::expected<std::vector<Issue>, Failure>
  {
    return Save(drawing, with)
        .and_then(
            [&file](Saved saved) -> std::expected<std::vector<Issue>, Failure>
            {
              std::ofstream to{file, std::ios::binary | std::ios::trunc};
              if (!to)
              {
                return std::unexpected{
                    Failure{.Code = FailureCode::Io,
                            .What = std::format("cannot write {}", file.string())}};
              }
              return Streamed(std::move(saved), to);
            });
  }
} // namespace DxfIO
