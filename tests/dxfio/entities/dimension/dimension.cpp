#include "entities/shapes/dimension/dimension.hpp"

#include "../../alike.hpp"
#include "../../kind.hpp"
#include "../../releases.hpp"
#include "../drawn.hpp"
#include "entities/io.hpp"
#include "file/cursor.hpp"

#include "dxfio/objects/mleader/style.hpp"

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO::Entities
{
namespace
{
auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
{
  return Drawn(self, release, "measures");
}

template <class KindT>
auto Alikened(const std::string &record, Version release = Version::R2018) -> KindT
{
  const KindT out = DxfIO::Read<KindT>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}

template <class FormT>
auto Formed(std::string_view self) -> FormT
{
  const Dimension one = Alikened<Dimension>(Marked(self));
  const auto *held = std::get_if<FormT>(&one.Figure->Form);
  REQUIRE(held != nullptr);
  return *held;
}
} // namespace

TEST_CASE("dimension: what every kind holds")
{
  const Dimension one = Alikened<Dimension>(Marked("2CD"));
  const Measure &all = one.Figure->Measure;
  CHECK(all.Block == "*D1");
  CHECK(all.Point == Vector{.X = 100, .Y = -20});
  CHECK(all.Flags == 32);
  CHECK(all.Attachment == 5);
  CHECK(all.Measured == 100);
  CHECK(all.Style == "ISO-25");
  CHECK(all.Normal == Up);
  CHECK(all.Text.empty());
}

TEST_CASE("dimension: every form, as the low bits of its flags say")
{
  CHECK(Formed<Forms::Linear>("2CD").End == Vector{.X = 100, .Y = 30});
  CHECK(Formed<Forms::Linear>("2DC").Angle == 90);
  CHECK(Formed<Forms::Aligned>("311").End == Vector{.X = 100, .Y = 30});
  CHECK(Formed<Forms::Angular>("32D").Start != Vector{});
  CHECK(Formed<Forms::Points>("345").Vertex != Vector{});
  CHECK(Formed<Forms::Diameter>("355").Leader == 0);
  CHECK(Formed<Forms::Radial>("36B").Chord != Vector{});
  CHECK(Formed<Forms::Ordinate>("390").Leader != Vector{});
  CHECK(Formed<Forms::Ordinate>("39B").Feature != Vector{});
}

TEST_CASE("dimension: its text, given or none")
{
  CHECK(Alikened<Dimension>(Marked("2F6")).Figure->Measure.Text == "20 mm");
  CHECK(Alikened<Dimension>(Marked("31E")).Figure->Measure.Text == "<> mm");
}

TEST_CASE("dimension: on paper, associative, kept as it came")
{
  CHECK(Alikened<Dimension>(Marked("3F3")).Record.Paper);
  CHECK_FALSE(Alikened<Dimension>(Marked("32D")).Record.Base.Reactors.empty());
}

TEST_CASE("dimension: R2007's flags and R2010's version gated")
{
  const std::string older = Marked("2CD", Version::R2004);
  CHECK_FALSE(older.contains(" 73\r\n"));
  CHECK(Alikened<Dimension>(older, Version::R2004).Figure->Measure.Version == 0);
  CHECK(Written(DxfIO::Read<Dimension>(Marked("2CD")), Version::R2004).find("280\r\n") == std::string::npos);
}

TEST_CASE("dimension: every release from R2000 alike")
{
  for (const Version release : Releases)
  {
    CAPTURE(release);
    CHECK(Alikened<Dimension>(Marked("2CD", release), release).Figure->Measure.Block == "*D1");
    CHECK(Alikened<Dimension>(Marked("355", release), release).Figure->Measure.Flags == 163);
  }
}

TEST_CASE("dimension: R12 holds no marker, its flags alone say the form")
{
  const Dimension one = Alikened<Dimension>(Marked("2CD", Version::R12), Version::R12);
  CHECK(std::holds_alternative<Forms::Linear>(one.Figure->Form));
  const Dimension round = Alikened<Dimension>(Marked("355", Version::R12), Version::R12);
  CHECK(std::holds_alternative<Forms::Diameter>(round.Figure->Form));
}

TEST_CASE("dimension: a form no flag names reads as a linear one")
{
  Dimension one = DxfIO::Read<Dimension>(Marked("2CD"));
  const std::string text = Written(one);
  const std::string seven =
      text.substr(0, text.find(" 70\r\n")) + " 70\r\n    39\r\n" + text.substr(text.find(" 71\r\n"));
  CHECK(std::holds_alternative<Forms::Linear>(DxfIO::Read<Dimension>(seven).Figure->Form));
}

TEST_CASE("dimension: its kind bits written from its form, whatever its flags say")
{
  Dimension one{};
  one.Figure->Measure.Flags = 32 + 3;
  one.Figure->Form = Forms::Radial{.Chord = Vector{.X = 5}, .Leader = 2};
  const auto back = DxfIO::Read<Dimension>(Written(one));
  CHECK(back.Figure->Measure.Flags == 32 + 4);
  REQUIRE(std::holds_alternative<Forms::Radial>(back.Figure->Form));
  CHECK(std::get<Forms::Radial>(back.Figure->Form) == std::get<Forms::Radial>(one.Figure->Form));
  one.Figure->Form = Forms::Linear{};
  CHECK(DxfIO::Read<Dimension>(Written(one)).Figure->Measure.Flags == 32);
}

TEST_CASE("dimension: of two 70s the first, as AutoCAD reads them")
{
  const std::string text = Written(DxfIO::Read<Dimension>(Marked("2CD")));
  const std::string after = " 71\r\n";
  const std::string two = text.substr(0, text.find(after)) + " 70\r\n    33\r\n" + text.substr(text.find(after));
  const auto all = Tags::All(two);
  REQUIRE(all);
  File::Cursor in{*all};
  const auto one = in.Take();
  REQUIRE(one);
  std::vector<Issue> issues;
  const Dimension held = Taken<Dimension>(*one, Fields::Reading{.Text = Encoding::Utf8(), .Issues = &issues});
  CHECK(held.Figure->Measure.Flags == 32);
  CHECK(std::holds_alternative<Forms::Linear>(held.Figure->Form));
  CHECK(held.Record.Base.Extra.empty());
  CHECK(issues.empty());
}

TEST_CASE("dimension: what a file leaves out, as AutoCAD takes it")
{
  const Dimension one{};
  CHECK(one.Figure->Measure.Attachment == MiddleCenter);
  CHECK(one.Figure->Measure.Measured == Unmeasured);
}

TEST_CASE("dimension: its text's attachment hides no multileader's")
{
  using namespace DxfIO::Objects::Mleader;
  CHECK(Objects::Mleader::Text{}.Top == Centered);
}

TEST_CASE("dimension: its block and style symbols, as a layer is")
{
  CHECK(std::is_same_v<decltype(Measure::Block), Symbol>);
  CHECK(std::is_same_v<decltype(Measure::Style), Symbol>);
}

TEST_CASE("dimension: its style in R12 and R14 too")
{
  for (const Version release : {Version::R12, Version::R14})
  {
    CAPTURE(release);
    Dimension one{};
    one.Figure->Measure.Style = Symbol{"ISO-25"};
    const std::string text = Written(one, release);
    CHECK(text.contains("  3\r\nISO-25\r\n"));
    CHECK(DxfIO::Read<Dimension>(text, release).Figure->Measure.Style == "ISO-25");
  }
}

TEST_CASE("dimension: a block or a style not given stays not given")
{
  Dimension one{};
  CHECK_FALSE(Written(one).contains("  2\r\n"));
  CHECK_FALSE(Written(one).contains("  3\r\n"));
}

TEST_CASE("dimension: a stray 40 of a linear one read and forgotten")
{
  const std::string text = Written(DxfIO::Read<Dimension>(Marked("2CD")));
  const std::string marker = "100\r\nAcDbRotatedDimension\r\n";
  const std::string stray = text.substr(0, text.find(marker)) + " 40\r\n1.0\r\n" + text.substr(text.find(marker));
  const Dimension held = DxfIO::Read<Dimension>(stray);
  CHECK(held.Record.Base.Extra.empty());
  CHECK(Written(held) == text);
}

TEST_CASE("dimension: an arc's, a type of its own, its picture kept")
{
  const ArcDimension one = Alikened<ArcDimension>(Marked("3B1"));
  CHECK(one.Bend->Measure.Block == "*D16");
  CHECK(one.Bend->Center != Vector{});
  CHECK(one.Picture.size() == 2092);
  const ArcDimension old = Alikened<ArcDimension>(Marked("3B1", Version::R2004), Version::R2004);
  CHECK(old.Picture.size() == 2064);
  Fields::Sheet page{Version::R2004};
  page.To.From = Version::R2018;
  REQUIRE(Given(page.To, one));
  CHECK_FALSE(page.Out.contains("310\r\n"));
}

TEST_CASE("dimension: a jogged radius, a type of its own")
{
  const LargeRadialDimension one = Alikened<LargeRadialDimension>(Marked("3CD"));
  CHECK(one.Jog->Chord != Vector{});
  CHECK_FALSE(one.Picture.empty());
}

TEST_CASE("dimension: named as DXF names each")
{
  CHECK(Named(Entity{Dimension{}}) == "DIMENSION");
  CHECK(Named(Entity{ArcDimension{}}) == "ARC_DIMENSION");
  CHECK(Named(Entity{LargeRadialDimension{}}) == "LARGE_RADIAL_DIMENSION");
}

TEST_CASE("dimension: a copy is its own, an entity stays small")
{
  Dimension one{};
  one.Figure->Measure.Flags = 1;
  Dimension two = one;
  two.Figure->Measure.Flags = 2;
  CHECK(one.Figure->Measure.Flags == 1);
  CHECK(sizeof(ArcDimension) < 2 * sizeof(Record));
}
} // namespace DxfIO::Entities
