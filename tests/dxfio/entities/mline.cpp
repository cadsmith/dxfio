#include "entities/shapes/mline.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../releases.hpp"
#include "drawn.hpp"
#include "entities/io.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Entities
{
namespace
{
auto Alikened(std::string_view self, Version release = Version::R2018) -> MLine
{
  const std::string record = Drawn(self, release, "mlines");
  const MLine out = DxfIO::Read<MLine>(record, release);
  CHECK(out.Record.Base.Extra.empty());
  CHECK(Alike(Written(out, release), record));
  return out;
}
} // namespace

TEST_CASE("mline: its joints and strands, in every release")
{
  for (const Version release : Releases)
  {
    const MLine one = Alikened("2CE", release);
    REQUIRE(one.Web->Joints.size() == 3);
    CHECK(one.Web->Style == "STANDARD");
    CHECK(one.Web->Scale == 20);
    CHECK(one.Web->Justification == 0);
    CHECK(one.Web->Lines == 2);
    const Joint &middle = one.Web->Joints.at(1);
    CHECK(middle.Position == Vector{.X = 10});
    REQUIRE(middle.Strands.size() == 2);
    CHECK(middle.Strands.at(1).Spans.at(0) == doctest::Approx(-28.28427));
  }
}

TEST_CASE("mline: closed, by its flags")
{
  CHECK(Alikened("2CF").Web->Flags == 3);
}

TEST_CASE("mline: a cut keeps its breaks in the strand it cuts")
{
  const MLine one = Alikened("2D1");
  CHECK(one.Web->Lines == 3);
  REQUIRE(one.Web->Joints.at(0).Strands.at(0).Spans.size() == 4);
  CHECK(one.Web->Joints.at(0).Strands.at(0).Spans.at(2) == doctest::Approx(4));
  CHECK(one.Web->Joints.at(0).Strands.at(0).Spans.at(3) == doctest::Approx(6));
}

TEST_CASE("mline: its normal given, on paper")
{
  CHECK(Alikened("2D3").Web->Normal.Y == -1);
  CHECK(Alikened("2E4").Record.Paper);
}

TEST_CASE("mline: its style a symbol, as a layer is")
{
  CHECK(std::is_same_v<decltype(Web::Style), Symbol>);
}
} // namespace DxfIO::Entities
