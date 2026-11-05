#include "../filing.hpp"
#include "chained.hpp"
#include "heading.hpp"
#include "seek.hpp"
#include "shapes/appid.hpp"
#include "shapes/block.hpp"
#include "shapes/dimstyle.hpp"
#include "shapes/layer.hpp"
#include "shapes/linetype.hpp"
#include "shapes/style.hpp"
#include "shapes/ucs.hpp"
#include "shapes/view.hpp"
#include "shapes/viewport.hpp"
#include "tables/io.hpp"

#include "dxfio/document.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace DxfIO::Tables
{
namespace
{
struct Filling
{
  Document *Drawing{nullptr};
  const Fields::Reading *With{nullptr};
};

struct Kind
{
  std::string_view Name;
  void (*Read)(const File::Nest &one, const Tables::Head &head, const Filling &into){nullptr};
  std::expected<void, Failure> (*Write)(Fields::Writing &to, const Document &from){nullptr};
};

template <class KindT>
auto Wrote(Fields::Writing &to, const Table<KindT> &one) -> std::expected<void, Failure>
{
  return Headed(to, one.Head)
      .and_then([&to, &one] -> std::expected<void, Failure>
                { return Chained(one.Records, [&to](const KindT &record) -> auto { return Given(to, record); }); })
      .and_then([&to] -> auto { return Fields::Word(to, 0, "ENDTAB"); });
}

template <class KindT>
auto Fits(const File::Record &record, const Tables::Head &head, const Filling &into) -> bool
{
  if constexpr (std::is_same_v<KindT, Unknown>)
  {
    return true;
  }
  else
  {
    if (record.Name == Type<KindT>)
    {
      return true;
    }
    into.With->Tell(Issue{
        .Line = record.Tags.front().Line,
        .What = std::format("record {} in table {}, left out", record.Name, head.Name),
    });
    return false;
  }
}

template <class KindT>
auto Tabled(const File::Nest &one, const Tables::Head &head, const Filling &into) -> Table<KindT>
{
  Table<KindT> out{.Head = head, .Records = {}};
  out.Records.reserve(one.Records.size());
  for (const File::Record &record : one.Records)
  {
    if (Fits<KindT>(record, head, into))
    {
      out.Records.push_back(Taken<KindT>(record, *into.With));
    }
  }
  return out;
}

template <class KindT, Table<KindT> Document::*Member>
void Filled(const File::Nest &one, const Tables::Head &head, const Filling &into)
{
  into.Drawing->*Member = Tabled<KindT>(one, head, into);
}

// Writes a table whose head is named, or whose records need it, then named for its kind.
template <class KindT, Table<KindT> Document::*Member>
auto Written(Fields::Writing &to, const Document &from) -> std::expected<void, Failure>
{
  const Table<KindT> &one = from.*Member;
  if (!one.Head.Name.empty())
  {
    return Wrote(to, one);
  }
  if (one.Records.empty())
  {
    return {};
  }
  Table<KindT> named = one;
  named.Head.Name = Type<KindT>;
  return Wrote(to, named);
}

void Loose(const File::Nest &one, const Tables::Head &head, const Filling &into)
{
  into.Drawing->Loose.push_back(Tabled<Unknown>(one, head, into));
}

template <class MemberT>
struct Listed;

template <class KindT>
struct Listed<Table<KindT> Document::*>
{
  using Type = KindT;
};

template <auto Member>
using Recorded = Listed<decltype(Member)>::Type;

template <auto Member>
constexpr Kind Entry{
    .Name = Type<Recorded<Member>>,
    .Read = Filled<Recorded<Member>, Member>,
    .Write = Written<Recorded<Member>, Member>,
};

constexpr std::array Kinds{
    Entry<&Document::Vport>, Entry<&Document::Ltype>,    Entry<&Document::Layer>,
    Entry<&Document::Style>, Entry<&Document::View>,     Entry<&Document::Ucs>,
    Entry<&Document::Appid>, Entry<&Document::Dimstyle>, Entry<&Document::BlockRecord>,
};

auto Named(std::string_view name) -> const Kind *
{
  return Seek(Kinds, name, &Kind::Name);
}

auto Strangers(Fields::Writing &to, const Document &drawing) -> std::expected<void, Failure>
{
  return Chained(drawing.Loose, [&](const Table<Unknown> &one) -> std::expected<void, Failure>
                 { return Named(one.Head.Name) == nullptr ? Wrote(to, one) : std::expected<void, Failure>{}; });
}
} // namespace

void Read(std::span<const File::Nest> all, Document &drawing, const Fields::Reading &with)
{
  const Filling into{.Drawing = &drawing, .With = &with};
  std::vector<std::string> seen;
  for (const File::Nest &one : all)
  {
    const Tables::Head head = Headed(one.Head, with);
    if (std::ranges::contains(seen, head.Name))
    {
      with.Tell(Issue{
          .Line = one.Head.Tags.front().Line,
          .What = std::format("table {} given again, the first kept", head.Name),
      });
      continue;
    }
    seen.push_back(head.Name);
    const Kind *found = Named(head.Name);
    const auto fill = found == nullptr ? Loose : found->Read;
    fill(one, head, into);
  }
}

auto Write(Fields::Writing &to, const Document &drawing) -> std::expected<void, Failure>
{
  return Chained(Kinds,
                 [&to, &drawing](const Kind &one) -> std::expected<void, Failure> { return one.Write(to, drawing); })
      .and_then([&to, &drawing] -> std::expected<void, Failure> { return Strangers(to, drawing); });
}
} // namespace DxfIO::Tables
