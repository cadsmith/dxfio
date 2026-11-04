#include "entities/shapes/mleader.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../releases.hpp"
#include "drawn.hpp"
#include "entities/io.hpp"

#include <string>
#include <variant>
#include <vector>

namespace DxfIO::Entities
{
namespace
{
auto Alikened(Version release = Version::R2018) -> MultiLeader
{
  const std::string record = Drawn("310", release, "kinds");
  const MultiLeader out = DxfIO::Read<MultiLeader>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}

auto Full() -> MultiLeader
{
  MultiLeader out{};
  out.Record.Base.Self = Handle{.Value = 0x2a};
  Callout &all = *out.Callout;
  all.Style = Handle{.Value = 0x12e};
  all.Arrow = Handle{.Value = 0x31};
  all.Linetype = Handle{.Value = 0x14};
  all.Lettering = Handle{.Value = 0x11};
  all.Block = Handle{.Value = 0x32};
  all.Tips = {Tip{.Index = 0, .Block = Handle{.Value = 0x33}}, Tip{.Index = 1, .Block = Handle{.Value = 0x34}}};
  all.Labels = {Label{.Definition = Handle{.Value = 0x35}, .Index = 3, .Width = 2.5, .Text = "TAG"}};
  Note &note = all.Context;
  note.Texted = true;
  note.Text = "two\\Plines";
  note.Style = Handle{.Value = 0x11};
  note.Heights = {1.5, 2.5};
  note.Blocked = true;
  note.Block = Handle{.Value = 0x36};
  note.Frame.Cells.fill(1.0);
  Stem stem{};
  stem.Points = {Vector{.X = 1}, Vector{.X = 2}};
  stem.Gaps = {Gap{.Index = 1, .Start = Vector{.Y = 1}}, Gap{.Index = 2, .End = Vector{.Y = 2}}};
  stem.Index = 4;
  stem.Color = 7;
  stem.Kind = 2;
  stem.Linetype = Handle{.Value = 0x37};
  stem.Lineweight = -1;
  stem.Arrow = 0.5;
  stem.Head = Handle{.Value = 0x38};
  stem.Overrides = 3;
  Branch branch{};
  branch.Spans = {Span{.Start = Vector{.X = 3}, .End = Vector{.X = 4}}};
  branch.Stems = {stem, Stem{.Points = {Vector{.Z = 1}}}};
  note.Branches = {branch, Branch{}};
  return out;
}
} // namespace

TEST_CASE("mleader: AutoCAD's note, its text and its one leader")
{
  const MultiLeader one = Alikened();
  const Callout &all = *one.Callout;
  const Note &note = all.Context;
  CHECK(all.Version == 2);
  CHECK(note.Scale == 1);
  CHECK(note.Texted);
  CHECK(note.Text == "note");
  CHECK(note.Location.X == 240);
  CHECK(note.Color == Inherited);
  CHECK_FALSE(note.Blocked);
  CHECK(note.Origin == Vector{.X = 220});
  REQUIRE(note.Branches.size() == 1);
  const Branch &branch = note.Branches.front();
  CHECK(branch.Last == Vector{.X = 230, .Y = 10});
  CHECK(branch.Length == 8);
  REQUIRE(branch.Stems.size() == 1);
  CHECK(branch.Stems.front().Points == std::vector{Vector{.X = 220}});
  CHECK_FALSE(branch.Stems.front().Color);
  CHECK(all.Style == Handle{.Value = 0x12e});
  CHECK(all.Overrides == 279552);
  CHECK(all.Size == 4);
  CHECK(all.Content == 2);
  CHECK(all.Lettering == Handle{.Value = 0x11});
  CHECK(all.Sized == Vector{.X = 1, .Y = 1, .Z = 1});
  CHECK(all.Tips.empty());
  CHECK(all.Labels.empty());
  CHECK_FALSE(all.Extended);
  CHECK_FALSE(one.Picture.empty());
}

TEST_CASE("mleader: every release alike, each with the groups it has")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened(release).Callout->Context.Text == "note");
  }
}

TEST_CASE("mleader: all it may hold comes back from every release")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    const MultiLeader full = Full();
    const std::string text = Written(full, release);
    const MultiLeader again = DxfIO::Read<MultiLeader>(text, release);
    CHECK(again.Record.Base.Extra.empty());
    CHECK(Written(again, release) == text);
    CHECK(again.Callout->Tips == full.Callout->Tips);
    CHECK(again.Callout->Labels == full.Callout->Labels);
    CHECK(again.Callout->Context.Branches == full.Callout->Context.Branches);
    CHECK(again.Callout->Context.Frame == full.Callout->Context.Frame);
  }
}

TEST_CASE("mleader: no text and no block, neither's groups written")
{
  MultiLeader one{};
  one.Callout->Context.Text = "dropped";
  const std::string text = Written(one);
  CHECK(text.find("dropped") == std::string::npos);
  CHECK(text.find("341\r\n") != std::string::npos);
  CHECK(DxfIO::Read<MultiLeader>(text).Callout->Context.Text.empty());
}

TEST_CASE("mleader: a block's transform a matrix, its 16 cells written")
{
  CHECK(std::is_same_v<decltype(Note::Frame), Matrix>);
  MultiLeader one{};
  one.Callout->Context.Blocked = true;
  const std::string text = Written(one);
  std::size_t cells = 0;
  for (std::size_t at = text.find(" 47\r\n"); at != std::string::npos; at = text.find(" 47\r\n", at + 1))
  {
    ++cells;
  }
  CHECK(cells == 16);
  CHECK(DxfIO::Read<MultiLeader>(text).Callout->Context.Frame == Identity);
}

TEST_CASE("mleader: an arrow and a block named only when there are")
{
  const std::string none = Written(MultiLeader{});
  CHECK(none.find("342\r\n") == std::string::npos);
  CHECK(none.find("344\r\n") == std::string::npos);
  const std::string full = Written(Full());
  CHECK(full.find("342\r\n") != std::string::npos);
  CHECK(full.find("344\r\n") != std::string::npos);
}

TEST_CASE("mleader: the braces close in the order they open")
{
  const std::string text = Written(Full());
  CHECK(text.find("CONTEXT_DATA{") < text.find("LEADER{"));
  CHECK(text.find("LEADER{") < text.find("LEADER_LINE{"));
  CHECK(text.find("305\r\n}") < text.find("303\r\n}"));
  CHECK(text.rfind("303\r\n}") < text.find("301\r\n}"));
}

TEST_CASE("mleader: a stray in its groups stays as it came, and is told")
{
  std::string text = Written(Full());
  const std::string dock = "297\r\n";
  text.insert(text.find(dock), " 77\r\n     1\r\n");
  const auto all = Tags::All(text);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  std::vector<Issue> issues;
  const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Unknown>(kept.front()));
  CHECK(issues.size() == 1);
}

TEST_CASE("mleader: a brace left open stays as it came")
{
  std::string text = Written(Full());
  const std::string close = "305\r\n}\r\n";
  text.erase(text.find(close), close.size());
  const auto all = Tags::All(text);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  const std::vector<File::Record> records{*one};
  const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8()});
  REQUIRE(kept.size() == 1);
  CHECK(std::holds_alternative<Unknown>(kept.front()));
}

TEST_CASE("mleader: alike only with what it holds alike")
{
  CHECK(Full() == Full());
  MultiLeader other = Full();
  other.Callout->Context.Branches.front().Stems.front().Gaps.front().Index = 9;
  CHECK(other != Full());
  other = Full();
  other.Callout->Context.Branches.front().Spans.front().End.X = 9;
  CHECK(other != Full());
  other = Full();
  other.Callout->Tips.front().Index = 9;
  CHECK(other != Full());
  other = Full();
  other.Callout->Labels.front().Text = "other";
  CHECK(other != Full());
}

TEST_CASE("mleader: named as DXF names it")
{
  CHECK(Named(Entity{MultiLeader{}}) == "MULTILEADER");
}
} // namespace DxfIO::Entities
