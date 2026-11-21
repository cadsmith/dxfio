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
      const MultiLeader out    = DxfIO::Read<MultiLeader>(record, release);
      CHECK(out.Record.Base.Extra.empty());
      CHECK(Alike(Written(out, release), record));
      return out;
    }

    auto Full() -> MultiLeader
    {
      MultiLeader out{};
      out.Record.Base.Self       = Handle{.Value = 0x2a};
      MultiLeaders::MLeader &all = *out.MLeader;
      all.Style                  = Handle{.Value = 0x12e};
      all.Arrowhead              = Handle{.Value = 0x31};
      all.Linetype               = Handle{.Value = 0x14};
      all.TextStyle              = Handle{.Value = 0x11};
      all.BlockContent           = Handle{.Value = 0x32};
      all.Arrowheads      = {MultiLeaders::Arrowhead{.Index = 0, .Block = Handle{.Value = 0x33}},
                             MultiLeaders::Arrowhead{.Index = 1, .Block = Handle{.Value = 0x34}}};
      all.BlockAttributes = {MultiLeaders::BlockAttribute{
          .Definition = Handle{.Value = 0x35}, .Index = 3, .Width = 2.5, .Text = "TAG"}};
      MultiLeaders::ContextData &note = all.Context;
      note.HasMText                   = true;
      note.Text                       = "two\\Plines";
      note.TextStyle                  = Handle{.Value = 0x11};
      note.ColumnHeights              = {1.5, 2.5};
      note.HasBlock                   = true;
      note.BlockContent               = Handle{.Value = 0x36};
      note.BlockTransform.Cells.fill(1.0);
      MultiLeaders::LeaderLine stem{};
      stem.Vertices      = {Vector{.X = 1}, Vector{.X = 2}};
      stem.Breaks        = {MultiLeaders::LineBreak{.Index = 1, .Start = Vector{.Y = 1}},
                            MultiLeaders::LineBreak{.Index = 2, .End = Vector{.Y = 2}}};
      stem.Index         = 4;
      stem.Color         = 7;
      stem.LeaderType    = 2;
      stem.Linetype      = Handle{.Value = 0x37};
      stem.Lineweight    = -1;
      stem.ArrowheadSize = 0.5;
      stem.Arrowhead     = Handle{.Value = 0x38};
      stem.Overrides     = 3;
      MultiLeaders::LeaderRoot branch{};
      branch.Breaks = {MultiLeaders::DoglegBreak{.Start = Vector{.X = 3}, .End = Vector{.X = 4}}};
      branch.Lines  = {stem, MultiLeaders::LeaderLine{.Vertices = {Vector{.Z = 1}}}};
      note.Leaders  = {branch, MultiLeaders::LeaderRoot{}};
      return out;
    }
  } // namespace

  TEST_CASE("mleader: AutoCAD's note, its text and its one leader")
  {
    const MultiLeader                one  = Alikened();
    const MultiLeaders::MLeader     &all  = *one.MLeader;
    const MultiLeaders::ContextData &note = all.Context;
    CHECK(all.Version       == 2);
    CHECK(note.ContentScale == 1);
    CHECK(note.HasMText);
    CHECK(note.Text           == "note");
    CHECK(note.TextLocation.X == 240);
    CHECK(note.TextColor      == Inherited);
    CHECK_FALSE(note.HasBlock);
    CHECK(note.PlaneOrigin      == Vector{.X = 220});
    REQUIRE(note.Leaders.size() == 1);
    const MultiLeaders::LeaderRoot &branch = note.Leaders.front();
    CHECK(branch.LastPoint      == Vector{.X = 230, .Y = 10});
    CHECK(branch.DoglegLength   == 8);
    REQUIRE(branch.Lines.size() == 1);
    CHECK(branch.Lines.front().Vertices == std::vector{Vector{.X = 220}});
    CHECK_FALSE(branch.Lines.front().Color);
    CHECK(all.Style         == Handle{.Value = 0x12e});
    CHECK(all.Overrides     == 279552);
    CHECK(all.ArrowheadSize == 4);
    CHECK(all.ContentType   == 2);
    CHECK(all.TextStyle     == Handle{.Value = 0x11});
    CHECK(all.BlockScale    == Vector{.X = 1, .Y = 1, .Z = 1});
    CHECK(all.Arrowheads.empty());
    CHECK(all.BlockAttributes.empty());
    CHECK_FALSE(all.ExtendToText);
    CHECK_FALSE(one.Picture.empty());
  }

  TEST_CASE("mleader: every release alike, each with the groups it has")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      CHECK(Alikened(release).MLeader->Context.Text == "note");
    }
  }

  TEST_CASE("mleader: all it may hold comes back from every release")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      const MultiLeader full  = Full();
      const std::string text  = Written(full, release);
      const MultiLeader again = DxfIO::Read<MultiLeader>(text, release);
      CHECK(again.Record.Base.Extra.empty());
      CHECK(Written(again, release)        == text);
      CHECK(again.MLeader->Arrowheads      == full.MLeader->Arrowheads);
      CHECK(again.MLeader->BlockAttributes == full.MLeader->BlockAttributes);
      CHECK(again.MLeader->Context.Leaders == full.MLeader->Context.Leaders);
      CHECK(again.MLeader->Context.BlockTransform == full.MLeader->Context.BlockTransform);
    }
  }

  TEST_CASE("mleader: no text and no block, neither's groups written")
  {
    MultiLeader one{};
    one.MLeader->Context.Text = "dropped";
    const std::string text    = Written(one);
    CHECK(text.find("dropped") == std::string::npos);
    CHECK(text.find("341\r\n") != std::string::npos);
    CHECK(DxfIO::Read<MultiLeader>(text).MLeader->Context.Text.empty());
  }

  TEST_CASE("mleader: a block's transform a matrix, its 16 cells written")
  {
    CHECK(std::is_same_v<decltype(MultiLeaders::ContextData::BlockTransform), Matrix>);
    MultiLeader one{};
    one.MLeader->Context.HasBlock = true;
    const std::string text        = Written(one);
    std::size_t       cells       = 0;
    for (std::size_t at = text.find(" 47\r\n"); at != std::string::npos;
         at             = text.find(" 47\r\n", at + 1))
    {
      ++cells;
    }
    CHECK(cells == 16);
    CHECK(DxfIO::Read<MultiLeader>(text).MLeader->Context.BlockTransform == Identity);
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
    std::string       text = Written(Full());
    const std::string dock = "297\r\n";
    text.insert(text.find(dock), " 77\r\n     1\r\n");
    const auto all = Tags::All(text);
    REQUIRE(all);
    File::Cursor in{*all};
    const auto   one = in.Take();
    REQUIRE(one);
    const std::vector<File::Record> records{*one};
    std::vector<Issue>              issues;
    const List                      kept =
        Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
    REQUIRE(kept.size() == 1);
    CHECK(std::holds_alternative<Unknown>(kept.front()));
    CHECK(issues.size() == 1);
  }

  TEST_CASE("mleader: a brace left open stays as it came")
  {
    std::string       text  = Written(Full());
    const std::string close = "305\r\n}\r\n";
    text.erase(text.find(close), close.size());
    const auto all = Tags::All(text);
    REQUIRE(all);
    File::Cursor in{*all};
    const auto   one = in.Take();
    REQUIRE(one);
    const std::vector<File::Record> records{*one};
    const List kept = Entities::Read(records, Fields::Reading{.Text = Encoding::Utf8()});
    REQUIRE(kept.size() == 1);
    CHECK(std::holds_alternative<Unknown>(kept.front()));
  }

  TEST_CASE("mleader: alike only with what it holds alike")
  {
    CHECK(Full() == Full());
    MultiLeader other                                                         = Full();
    other.MLeader->Context.Leaders.front().Lines.front().Breaks.front().Index = 9;
    CHECK(other != Full());
    other                                                       = Full();
    other.MLeader->Context.Leaders.front().Breaks.front().End.X = 9;
    CHECK(other != Full());
    other                                   = Full();
    other.MLeader->Arrowheads.front().Index = 9;
    CHECK(other != Full());
    other                                       = Full();
    other.MLeader->BlockAttributes.front().Text = "other";
    CHECK(other != Full());
  }

  TEST_CASE("mleader: named as DXF names it")
  {
    CHECK(Named(Entity{MultiLeader{}}) == "MULTILEADER");
  }
} // namespace DxfIO::Entities
