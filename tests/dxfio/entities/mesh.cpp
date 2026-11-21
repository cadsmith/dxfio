#include "entities/shapes/mesh.hpp"

#include "../alike.hpp"
#include "../kind.hpp"
#include "../releases.hpp"
#include "drawn.hpp"
#include "entities/io.hpp"

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace DxfIO::Entities
{
  namespace
  {
    auto Sampled(Version release = Version::R2018) -> std::string
    {
      return Drawn("2D9", release, "kinds");
    }

    auto Alikened(const std::string &record, Version release = Version::R2018) -> Mesh
    {
      const Mesh out = DxfIO::Read<Mesh>(record, release);
      CHECK(out.Record.Base.Extra.empty());
      CHECK(Alike(Written(out, release), record));
      return out;
    }

    auto Held(std::string_view body) -> std::string
    {
      return "  0\r\nMESH\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n"
             "  8\r\n0\r\n100\r\nAcDbSubDMesh\r\n"
           + std::string{body};
    }

    constexpr std::string_view Small = " 71\r\n     2\r\n 72\r\n     1\r\n 91\r\n        2\r\n"
                                       " 92\r\n        3\r\n"
                                       " 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
                                       " 10\r\n1.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
                                       " 10\r\n0.0\r\n 20\r\n1.0\r\n 30\r\n0.0\r\n"
                                       " 93\r\n        4\r\n"
                                       " 90\r\n        3\r\n 90\r\n        0\r\n"
                                       " 90\r\n        1\r\n 90\r\n        2\r\n"
                                       " 94\r\n        1\r\n"
                                       " 90\r\n        0\r\n 90\r\n        1\r\n"
                                       " 95\r\n        1\r\n"
                                       "140\r\n0.5\r\n";
  } // namespace

  TEST_CASE("mesh: AutoCAD's box, its vertices, faces and edges")
  {
    const Mesh one = Alikened(Sampled());
    CHECK(one.Data->Version          == 2);
    CHECK(one.Data->Blend            == 0);
    CHECK(one.Data->Level            == 0);
    CHECK(one.Data->Vertices.size()  == 56);
    CHECK(one.Data->Vertices.front() == Vector{.X = 100, .Y = 0, .Z = 10});
    CHECK(one.Data->Faces.size()     == 270);
    CHECK(one.Data->Faces.front()    == 4);
    CHECK(one.Data->Edges.size() % 2 == 0);
    CHECK(one.Data->Creases.size()   == one.Data->Edges.size() / 2);
    CHECK(one.Data->Overridden       == 0);
    CHECK(one.Data->Overrides.empty());
    CHECK_FALSE(one.Picture.empty());
  }

  TEST_CASE("mesh: every release alike, the picture under its own code")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      CHECK(Alikened(Sampled(release), release).Data->Vertices.size() == 56);
    }
  }

  TEST_CASE("mesh: a small one by hand, each list its own count")
  {
    const Mesh one = Replayed<Mesh>(Held(std::string{Small} + " 90\r\n        0\r\n"));
    CHECK(one.Data->Blend   == 1);
    CHECK(one.Data->Level   == 2);
    CHECK(one.Data->Faces   == std::vector{3, 0, 1, 2});
    CHECK(one.Data->Edges   == std::vector{0, 1});
    CHECK(one.Data->Creases == std::vector{0.5});
  }

  TEST_CASE("mesh: overrides kept in their order, every group")
  {
    const std::string text = Held(std::string{Small}
                                  + " 90\r\n        1\r\n"
                                    " 91\r\n        7\r\n"
                                    " 92\r\n        1\r\n"
                                    " 90\r\n        0\r\n"
                                    " 63\r\n     3\r\n");
    const Mesh        one  = Replayed<Mesh>(text);
    CHECK(one.Data->Overridden             == 1);
    REQUIRE(one.Data->Overrides.size()     == 4);
    CHECK(one.Data->Overrides.front().Code == 91);
    CHECK(one.Data->Overrides.back().Code  == 63);
  }

  TEST_CASE("mesh: nothing in it")
  {
    const Mesh one =
        Replayed<Mesh>(Held(" 71\r\n     2\r\n 72\r\n     0\r\n 91\r\n        0\r\n"
                            " 92\r\n        0\r\n 93\r\n        0\r\n 94\r\n        0\r\n"
                            " 95\r\n        0\r\n 90\r\n        0\r\n"));
    CHECK(one.Data->Vertices.empty());
    CHECK(one.Data->Faces.empty());
    CHECK(one.Data->Edges.empty());
  }

  TEST_CASE("mesh: a count that lies is written true")
  {
    std::string       text = Held(std::string{Small} + " 90\r\n        0\r\n");
    const std::string told = " 92\r\n        3\r\n";
    text.replace(text.find(told), told.size(), " 92\r\n        9\r\n");
    const Mesh one = DxfIO::Read<Mesh>(text);
    CHECK(one.Data->Vertices.size() == 3);
    CHECK(Written(one).find(told) != std::string::npos);
  }

  TEST_CASE("mesh: a count of edges that lies is written true")
  {
    std::string       text = Held(std::string{Small} + " 90\r\n        0\r\n");
    const std::string told = " 94\r\n        1\r\n";
    text.replace(text.find(told), told.size(), " 94\r\n        4\r\n");
    const Mesh one = DxfIO::Read<Mesh>(text);
    CHECK(one.Data->Edges == std::vector{0, 1});
    CHECK(Written(one).contains(told));
  }

  TEST_CASE("mesh: an edge of one vertex stays as it came, and is told")
  {
    std::string       text    = Held(std::string{Small} + " 90\r\n        0\r\n");
    const std::string creases = " 95\r\n";
    text.insert(text.find(creases), " 90\r\n        2\r\n");
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

  TEST_CASE("mesh: an edge of one vertex not written")
  {
    Mesh one{};
    one.Data->Edges = {0, 1, 2};
    Fields::Sheet page{};
    const auto    written = Given(page.To, one);
    REQUIRE_FALSE(written);
    CHECK(written.error().What == "a MESH's edges hold an odd count of vertex indices");
  }

  TEST_CASE("mesh: a stray among its lists stays as it came, and is told")
  {
    std::string       text  = Held(std::string{Small} + " 90\r\n        0\r\n");
    const std::string faces = " 93\r\n";
    text.insert(text.find(faces), " 77\r\n     1\r\n");
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

  TEST_CASE("mesh: no count of overrides, none")
  {
    const Mesh one = DxfIO::Read<Mesh>(Held(Small));
    CHECK(one.Data->Overridden == 0);
    CHECK(Written(one).ends_with(" 90\r\n        0\r\n"));
  }

  TEST_CASE("mesh: a copy is its own, its box deep")
  {
    Mesh one{};
    one.Data->Level = 1;
    Mesh two        = one;
    two.Data->Level = 2;
    CHECK(one.Data->Level == 1);
  }

  TEST_CASE("mesh: named as DXF names it")
  {
    CHECK(Named(Entity{Mesh{}}) == "MESH");
  }
} // namespace DxfIO::Entities
