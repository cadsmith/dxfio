#include "entities/shapes/mtext.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "drawn.hpp"

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Entities
{
  namespace
  {
    auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
    {
      return Drawn(self, release, "mtexts");
    }

    auto Alikened(const std::string &record, Version release = Version::R2018) -> MText
    {
      const MText out = DxfIO::Read<MText>(record, release);
      CHECK(Alike(Written(out, release), record));
      return out;
    }

    auto Held(std::string_view body) -> std::string
    {
      return "  0\r\nMTEXT\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
             "  8\r\n0\r\n100\r\nAcDbMText\r\n"
           + std::string{body};
    }

    constexpr std::string_view Whole =
        " 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n 40\r\n2.5\r\n 41\r\n0.0\r\n"
        " 46\r\n0.0\r\n 71\r\n     1\r\n 72\r\n     5\r\n  1\r\nx\r\n"
        " 73\r\n     1\r\n 44\r\n1.0\r\n";

    constexpr std::string_view Block =
        "101\r\nEmbedded Object\r\n 70\r\n     1\r\n 10\r\n1.0\r\n 20\r\n0.0\r\n"
        " 30\r\n0.0\r\n 11\r\n0.0\r\n 21\r\n0.0\r\n 31\r\n0.0\r\n 40\r\n30.0\r\n"
        " 41\r\n0.0\r\n 42\r\n8.0\r\n 43\r\n3.0\r\n 71\r\n     2\r\n"
        " 72\r\n     1\r\n 44\r\n30.0\r\n 45\r\n12.5\r\n 73\r\n     0\r\n"
        " 74\r\n     0\r\n 46\r\n0.0\r\n";
  } // namespace

  TEST_CASE("mtext: as AutoCAD makes it from a file, every group written")
  {
    const MText one = Alikened(Marked("2D4"));
    CHECK(one.Record.Base.Extra.empty());
    CHECK(one.Data->Position         == Vector{.X = 80});
    CHECK(one.Data->Height           == 1.5);
    CHECK(one.Data->Width            == 30);
    CHECK(one.Data->Defined          == 0);
    CHECK(one.Data->Attachment       == 2);
    CHECK(one.Data->Flow             == 5);
    CHECK(one.Data->Value            == "attached");
    CHECK(one.Data->Style            == "Standard");
    CHECK(one.Data->Extrusion        == Up);
    CHECK(one.Data->LineSpacingStyle == 1);
    CHECK(one.Data->LineSpacingFactor == 1);
    CHECK_FALSE(one.Data->Direction);
    CHECK_FALSE(one.Data->Rotation);
    CHECK(one.Data->Fill == 0);
    CHECK_FALSE(one.Data->Columns);
  }

  TEST_CASE("mtext: every attachment and way of running")
  {
    constexpr std::array selves{"2D3", "2D4", "2D5", "2D6", "2D7", "2D8", "2D9", "2DA", "2DB"};
    int                  attachment = 0;
    for (const char *const self : selves)
    {
      CHECK(Alikened(Marked(self)).Data->Attachment == ++attachment);
    }
    CHECK(Alikened(Marked("2DC")).Data->Flow == 1);
    CHECK(Alikened(Marked("2DD")).Data->Flow == 3);
    CHECK(Alikened(Marked("2DE")).Data->Flow == 5);
  }

  TEST_CASE("mtext: its line spacing, its turn and what it stands in")
  {
    const MText least = Alikened(Marked("2DF"));
    CHECK(least.Data->LineSpacingStyle  == 1);
    CHECK(least.Data->LineSpacingFactor == 1.5);
    const MText exact = Alikened(Marked("2E0"));
    CHECK(exact.Data->LineSpacingStyle  == 2);
    CHECK(exact.Data->LineSpacingFactor == 0.5);
    REQUIRE(Alikened(Marked("2E2")).Data->Direction);
    CHECK(*Alikened(Marked("2E2")).Data->Direction == Vector{.Y = 1});
    CHECK(Alikened(Marked("2E4")).Data->Extrusion  == Vector{.Z = -1});
    CHECK(Alikened(Marked("2EA")).Data->Defined    == 12);
    CHECK(Alikened(Marked("2EB")).Data->Style      == "tall");
  }

  TEST_CASE("mtext: a background's color book as an entity's, none until given")
  {
    CHECK(std::is_same_v<decltype(MTextData::Book), decltype(Record::Book)>);
    MText one{};
    one.Data->Fill = 1;
    CHECK_FALSE(Written(one).contains("431\r\n"));
    one.Data->Book = "DIC COLOR GUIDE(R)$DIC 1";
    CHECK(DxfIO::Read<MText>(Written(one)).Data->Book == one.Data->Book);
  }

  TEST_CASE("mtext: every group a text can hold, in AutoCAD's order")
  {
    const MText one = Alikened(Marked("2EC"));
    CHECK(one.Record.Base.Extra.empty());
    CHECK(one.Data->Style     == "tall");
    CHECK(one.Data->Extrusion == Vector{.Z = -1});
    REQUIRE(one.Data->Direction);
    CHECK(one.Data->LineSpacingStyle == 2);
    CHECK(one.Data->Fill             == 1);
    CHECK(one.Data->Color            == 5);
    CHECK(one.Data->Rgb          == 255);
    CHECK(one.Data->Book         == "DIC COLOR GUIDE(R)$DIC 1");
    CHECK(one.Data->Margin       == 2);
    CHECK(one.Data->Transparency == 33554559);
  }

  TEST_CASE("mtext: its background, what each bit brings with it")
  {
    const MText filled = Alikened(Marked("2E5"));
    CHECK(filled.Data->Fill         == 1);
    CHECK(filled.Data->Color        == 1);
    CHECK(filled.Data->Margin       == 1.5);
    CHECK(filled.Data->Transparency == 0);
    CHECK(Alikened(Marked("2E6")).Data->Fill == 3);
    const MText truly = Alikened(Marked("2E7"));
    CHECK(truly.Data->Rgb          == 16711680);
    CHECK(truly.Data->Transparency == 128);
    const MText framed = Alikened(Marked("2E8"));
    CHECK(framed.Data->Fill  == 16);
    CHECK(framed.Data->Color == 256);
    const MText alone = Alikened(Marked("2F6"));
    CHECK(alone.Data->Fill == 2);
    CHECK_FALSE(alone.Data->Color);
    CHECK_FALSE(alone.Data->Margin);
    CHECK_FALSE(alone.Data->Transparency);
  }

  TEST_CASE("mtext: a framed text keeps only its flag at R2013")
  {
    const MText framed = Alikened(Marked("2E8", Version::R2013), Version::R2013);
    CHECK(framed.Data->Fill == 16);
    CHECK_FALSE(framed.Data->Color);
    CHECK(Alikened(Marked("2E9", Version::R2010), Version::R2010).Data->Fill == 1);
  }

  TEST_CASE("mtext: long ones cut as AutoCAD cuts them")
  {
    for (const Version release : {Version::R2000, Version::R2004, Version::R2007, Version::R2018})
    {
      CAPTURE(release);
      const std::string record = Marked("2ED", release);
      CHECK(Written(DxfIO::Read<MText>(record, release), release)
                .contains(record.substr(record.find("  3\r\n"), 520)));
    }
    CHECK(Alikened(Marked("2ED")).Data->Value.size() == 503);
    const std::string wide = Alikened(Marked("2EE")).Data->Value;
    CHECK(wide.starts_with("caf\xc3\xa9 "));
    CHECK(Alikened(Marked("2EF")).Data->Value.size() == 963);
    const std::string carets = Alikened(Marked("55F")).Data->Value;
    CHECK(carets.size() == 483);
    CHECK(carets.ends_with("^end"));
  }

  TEST_CASE("mtext: empty, blank, and inline codes left as they are")
  {
    CHECK(Alikened(Marked("2F4")).Data->Value.empty());
    CHECK(Alikened(Marked("2F5")).Data->Value == " ");
    CHECK(Alikened(Marked("2F1")).Data->Value.starts_with("{\\fArial|b1"));
    CHECK(Alikened(Marked("2F3")).Data->Value.starts_with("caret ^ and"));
  }

  TEST_CASE("mtext: columns as one record from R2018")
  {
    const MText fixed = Alikened(Marked("2FB"));
    CHECK(fixed.Record.Base.Extra.empty());
    CHECK(fixed.Data->Width   == 50);
    CHECK(fixed.Data->Defined == 20);
    REQUIRE(fixed.Data->Columns);
    CHECK(fixed.Data->Columns->Kind    == 1);
    CHECK(fixed.Data->Columns->Count   == 2);
    CHECK(fixed.Data->Columns->Breadth == 20);
    CHECK(fixed.Data->Columns->Gutter  == 10);
    CHECK_FALSE(fixed.Data->Columns->Automatic);
    CHECK(fixed.Data->Columns->Heights.empty());
    const MText dynamic = Alikened(Marked("2FC"));
    REQUIRE(dynamic.Data->Columns);
    CHECK(dynamic.Data->Columns->Kind  == 2);
    CHECK(dynamic.Data->Columns->Count == 16);
    CHECK(dynamic.Data->Columns->Automatic);
    CHECK(dynamic.Data->Columns->Position == Vector{.X = 80, .Y = 160});
  }

  TEST_CASE("mtext: the object's own height for each dynamic column")
  {
    const MText one = Alikened(Marked("2D0"));
    REQUIRE(one.Data->Columns);
    CHECK(one.Data->Columns->Count   == 1);
    CHECK(one.Data->Columns->Heights == std::vector{0.0});
  }

  TEST_CASE("mtext: the object's first group, the text's attachment")
  {
    const MText one = Alikened(Marked("2FD"));
    CHECK(one.Data->Attachment == 5);
    REQUIRE(one.Data->Columns);
    CHECK(one.Data->Columns->Attachment == 5);
  }

  TEST_CASE("mtext: annotative, what it keeps elsewhere kept")
  {
    const MText one = Alikened(Marked("2FE"));
    CHECK(one.Data->Style == "small");
    CHECK(one.Record.Base.Dictionary != Handle{});
    CHECK_FALSE(one.Record.Base.Extended.empty());
  }

  TEST_CASE("mtext: extended data kept, the numbers below R2007 as they came")
  {
    const MText one = Alikened(Marked("2D4", Version::R2000), Version::R2000);
    CHECK_FALSE(one.Record.Base.Extended.empty());
    CHECK(one.Data->Attachment == 2);
    CHECK(Alikened(Marked("2EA", Version::R2004), Version::R2004).Data->Fill == 0);
  }

  TEST_CASE("mtext: of a direction and a rotation the one given last")
  {
    const std::string text      = "  1\r\nx\r\n";
    const std::string direction = " 11\r\n0.0\r\n 21\r\n1.0\r\n 31\r\n0.0\r\n";
    const std::string rotation  = " 50\r\n30.0\r\n";
    const MText       first     = DxfIO::Read<MText>(Held(text + rotation + direction));
    CHECK(first.Data->Direction);
    CHECK_FALSE(first.Data->Rotation);
    CHECK(first.Record.Base.Extra.empty());
    const MText second = DxfIO::Read<MText>(Held(text + direction + rotation));
    CHECK_FALSE(second.Data->Direction);
    CHECK(second.Data->Rotation == 30);
    CHECK(Written(second).contains(" 50\r\n30.0\r\n"));
    CHECK_FALSE(Written(second).contains(" 11\r\n"));
  }

  TEST_CASE("mtext: what a record leaves out, AutoCAD's defaults written")
  {
    const MText one = DxfIO::Read<MText>(Held(""));
    CHECK(one.Data->Attachment       == 1);
    CHECK(one.Data->Flow             == 5);
    CHECK(one.Data->LineSpacingStyle == 1);
    CHECK(one.Data->LineSpacingFactor == 1);
    CHECK(one.Data->Value.empty());
    CHECK(Alike(Written(one), Held(" 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
                                   " 40\r\n0.0\r\n 41\r\n0.0\r\n 46\r\n0.0\r\n"
                                   " 71\r\n     1\r\n 72\r\n     5\r\n"
                                   "  1\r\n\r\n 73\r\n     1\r\n 44\r\n1.0\r\n")));
  }

  TEST_CASE("mtext: pieces wherever the record gives them")
  {
    const MText one =
        DxfIO::Read<MText>(Held("  3\r\nhead\r\n 10\r\n1.0\r\n  1\r\nmid\r\n 73\r\n     1\r\n"
                                "  1\r\nend\r\n"));
    CHECK(one.Data->Value == "headmidend");
    CHECK(one.Record.Base.Extra.empty());
  }

  TEST_CASE("mtext: a value out of range stays what the record says")
  {
    const MText one = DxfIO::Read<MText>(Held(" 71\r\n    10\r\n 72\r\n     2\r\n 73\r\n     3\r\n"
                                              " 44\r\n9.0\r\n 40\r\n-1.0\r\n"));
    CHECK(one.Data->Attachment       == 10);
    CHECK(one.Data->Flow             == 2);
    CHECK(one.Data->LineSpacingStyle == 3);
    CHECK(one.Data->LineSpacingFactor == 9);
    CHECK(one.Data->Height            == -1);
  }

  TEST_CASE("mtext: what AutoCAD reads as columns kept with no object")
  {
    const MText one = DxfIO::Read<MText>(Held(std::string{Whole} + " 75\r\n     1\r\n"));
    REQUIRE(one.Record.Base.Extra.size()     == 1);
    CHECK(one.Record.Base.Extra.front().Code == 75);
  }

  TEST_CASE("mtext: dropped with one, as AutoCAD refuses them past the object")
  {
    for (const std::string_view code :
         {" 49\r\n1.0", " 75\r\n     1", " 76\r\n     1", " 78\r\n     1", " 79\r\n     1"})
    {
      CAPTURE(code);
      const std::string text =
          Held(std::string{Whole} + std::string{code} + "\r\n 39\r\n1.0\r\n" + std::string{Block});
      const auto all = Tags::All(text);
      REQUIRE(all);
      File::Cursor in{*all};
      const auto   taken = in.Take();
      REQUIRE(taken);
      std::vector<Issue> issues;
      const MText        one =
          DxfIO::Taken<MText>(*taken, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
      REQUIRE(one.Data->Columns);
      CHECK(issues.size() == 1);
      REQUIRE(one.Record.Base.Extra.size()     == 1);
      CHECK(one.Record.Base.Extra.front().Code == 39);
      CHECK_FALSE(Written(one).contains(code));
    }
  }

  TEST_CASE("mtext: a group left out before the record stays out of the object")
  {
    const std::string text =
        Held(std::string{Whole} + " 90\r\n3000000000\r\n" + std::string{Block});
    std::vector<Issue> issues;
    const auto         all = Tags::All(text, &issues);
    REQUIRE(all);
    File::Cursor in{*all};
    const auto   taken = in.Take();
    REQUIRE(taken);
    const MText one =
        DxfIO::Taken<MText>(*taken, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
    CHECK(issues.size() == 1);
    CHECK(one.Record.Base.Extra.empty());
  }

  TEST_CASE("mtext: the object before R2018 kept as it came, else not written")
  {
    const std::string text  = Held(std::string{Whole} + std::string{Block});
    const MText       older = Alikened(text, Version::R2013);
    CHECK_FALSE(older.Data->Columns);
    CHECK(older.Record.Base.Extra.size() == 19);
    CHECK(Alikened(text).Data->Columns);
  }

  TEST_CASE("mtext: a frame's groups left out below R2018, a fill's kept")
  {
    const MText       framed = DxfIO::Read<MText>(Marked("2E8"));
    const std::string older  = Written(framed, Version::R2013);
    CHECK(older.contains(" 90\r\n       16\r\n"));
    CHECK_FALSE(older.contains(" 63\r\n"));
    CHECK_FALSE(older.contains(" 45\r\n"));
    CHECK(Written(DxfIO::Read<MText>(Marked("2E5")), Version::R2013).contains(" 63\r\n"));
  }

  TEST_CASE("mtext: fields of later releases left out of an older one")
  {
    const MText       one   = DxfIO::Read<MText>(Marked("2EC"));
    const std::string older = Written(one, Version::R2000);
    CHECK_FALSE(older.contains(" 46\r\n"));
    CHECK_FALSE(older.contains(" 90\r\n"));
    CHECK_FALSE(older.contains("431\r\n"));
    CHECK(older.contains(" 73\r\n"));
  }
} // namespace DxfIO::Entities
