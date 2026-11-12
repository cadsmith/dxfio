#include "entities/shapes/insert.hpp"

#include "../kind.hpp"
#include "../written.hpp"
#include "drawn.hpp"
#include "heavy.hpp"

#include "dxfio/save.hpp"

#include <algorithm>
#include <initializer_list>
#include <string>
#include <string_view>
#include <variant>

namespace DxfIO::Entities
{
  namespace
  {
    auto Marked(std::string_view self, Version release = Version::R2018) -> std::string
    {
      return Drawn(self, release, "inserts");
    }

    auto Found(const List &all, std::uint64_t self) -> const Insert &
    {
      return Traced<Insert>(all, self);
    }

    constexpr std::string_view Plain =
        "  0\r\nINSERT\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n  "
        "8\r\n0\r\n"
        "100\r\nAcDbBlockReference\r\n  2\r\nb\r\n"
        " 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n";

    constexpr std::string_view Open =
        "  0\r\nINSERT\r\n  5\r\n2A\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n  "
        "8\r\n0\r\n"
        "100\r\nAcDbBlockReference\r\n 66\r\n     1\r\n  2\r\nb\r\n"
        " 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n";

    constexpr std::string_view Tagged =
        "  0\r\nATTRIB\r\n  5\r\n2B\r\n330\r\n2A\r\n100\r\nAcDbEntity\r\n  "
        "8\r\n0\r\n"
        "100\r\nAcDbText\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
        " 40\r\n1.0\r\n  1\r\nx\r\n100\r\nAcDbAttribute\r\n280\r\n     0\r\n"
        "  2\r\nT\r\n 70\r\n     0\r\n280\r\n     0\r\n";

    constexpr std::string_view Closing = "  0\r\nSEQEND\r\n  5\r\n2C\r\n330\r\n2A\r\n"
                                         "100\r\nAcDbEntity\r\n  8\r\n0\r\n";

    constexpr std::string_view Stray =
        "  0\r\nLINE\r\n  5\r\n2E\r\n330\r\n1F\r\n100\r\nAcDbEntity\r\n  8\r\n0\r\n"
        "100\r\nAcDbLine\r\n 10\r\n0.0\r\n 20\r\n0.0\r\n 30\r\n0.0\r\n"
        " 11\r\n1.0\r\n 21\r\n1.0\r\n 31\r\n0.0\r\n";

    auto Joined(std::initializer_list<std::string_view> parts) -> std::string
    {
      std::string out;
      for (const std::string_view part : parts)
      {
        out += part;
      }
      return out;
    }
  } // namespace

  TEST_CASE("insert: a plain one, no 66 and nothing after it")
  {
    const Insert taken = Replayed<Insert>(Marked("2DA"));
    CHECK(taken.Block == "plain");
    CHECK_FALSE(taken.Follows);
    CHECK(taken.Scale == Vector{.X = 1, .Y = 1, .Z = 1});
    CHECK_FALSE(taken.Columns);
    CHECK(taken.Attributes.Children.empty());
    CHECK_FALSE(taken.Attributes.Closing);
  }

  TEST_CASE("insert: scaled and turned, each scale left out on its own")
  {
    const Insert taken = Replayed<Insert>(Marked("2DB"));
    CHECK(taken.Scale        == Vector{.X = 2, .Y = 3, .Z = 2});
    CHECK(taken.Rotation     == 45);
    CHECK(taken.Record.Layer == "walls");
    const Insert mirrored = Replayed<Insert>(Marked("2DC"));
    CHECK(mirrored.Scale      == Vector{.X = -1, .Y = 1, .Z = 1});
    CHECK(mirrored.Position.Z == 5);
    CHECK(Replayed<Insert>(Marked("2DD")).Extrusion.Y == -1);
  }

  TEST_CASE("insert: a MINSERT, its marker and both counts, 1 too")
  {
    const Insert grid = Replayed<Insert>(Marked("2DE"));
    CHECK(grid.Columns == 3);
    CHECK(grid.Rows    == 2);
    CHECK(grid.Across  == 5);
    CHECK(grid.Down    == 4);
    const Insert row = Replayed<Insert>(Marked("2DF"));
    CHECK(row.Rows == 1);
    CHECK(row.Down == 0);
    const Insert column = Replayed<Insert>(Marked("2E0"));
    CHECK(column.Columns == 1);
    CHECK(column.Across  == 0);
  }

  TEST_CASE("insert: an array of one is a plain reference, as AutoCAD writes it")
  {
    CHECK_FALSE(Replayed<Insert>(Marked("2F1")).Columns);
  }

  TEST_CASE("insert: its rotation before its counts, its extrusion after them")
  {
    const Insert taken = Replayed<Insert>(Marked("2F2"));
    CHECK(taken.Follows);
    CHECK(taken.Rotation    == 30);
    CHECK(taken.Columns     == 2);
    CHECK(taken.Extrusion.Y == -1);
  }

  TEST_CASE("insert: its attributes and SEQEND its own")
  {
    const DxfIO::Document file  = Opened("inserts", Version::R2018);
    const Insert         &taken = Found(file.Entities, 0x2e1);
    CHECK(taken.Follows);
    REQUIRE(taken.Attributes.Children.size()    == 2);
    CHECK(taken.Attributes.Children.front().Tag == "ONE");
    CHECK(taken.Attributes.Children.back().Tag  == "TWO");
    REQUIRE(taken.Attributes.Closing);
    CHECK((*taken.Attributes.Closing)->Record.Base.Owner == Handle{0x2e1});
  }

  TEST_CASE("insert: extended data on it and on an attribute")
  {
    const DxfIO::Document file  = Opened("inserts", Version::R2018);
    const Insert         &taken = Found(file.Entities, 0x2fb);
    CHECK(taken.Record.Base.Extended.size() == 1);
    CHECK(taken.Attributes.Children.front().Record.Base.Extended.size() == 1);
  }

  TEST_CASE("insert: with attributes in a block, in paper space, in every release")
  {
    for (const Version release : Releases)
    {
      CAPTURE(release);
      const DxfIO::Document file = Opened("inserts", release);
      const auto            nest =
          std::ranges::find(file.Blocks, "nest",
                            [](const Blocks::Block &one) -> std::string { return one.Begin.Name; });
      REQUIRE(nest != file.Blocks.end());
      REQUIRE(nest->Entities.size() == 1);
      CHECK(std::get<Insert>(nest->Entities.front()).Attributes.Children.size() == 1);
      CHECK(Found(file.Entities, 0x316).Record.Paper);
    }
  }

  TEST_CASE("insert: R12's the same, with no markers and nothing left over")
  {
    const auto read = Load(Acad(Fixture("inserts", Version::R12)));
    REQUIRE(read);
    CHECK(read->Issues.empty());
    const List &all = read->Document.Entities;
    CHECK(Found(all, 0x2de).Columns == 3);
    CHECK(Found(all, 0x2e1).Attributes.Children.size() == 2);
    for (const Entity &one : all)
    {
      if (const auto *held = std::get_if<Insert>(&one))
      {
        CHECK(held->Record.Base.Extra.empty());
        for (const Attrib &child : held->Attributes.Children)
        {
          CHECK(child.Record.Base.Extra.empty());
        }
      }
    }
  }

  TEST_CASE("insert: its 66 makes what follows its own, as AutoCAD will have it")
  {
    const auto read = Load(Drawing(Joined({Plain, Tagged, Closing})));
    REQUIRE(read);
    const List &all = read->Document.Entities;
    REQUIRE(all.size() == 3);
    CHECK(std::get<Insert>(all.at(0)).Attributes.Children.empty());
    CHECK(std::get<Unknown>(all.at(1)).Type == "ATTRIB");
    CHECK(std::get<Unknown>(all.at(2)).Type == "SEQEND");
    CHECK(read->Issues.empty());
  }

  TEST_CASE("insert: a 66 with nothing after it told, and its 66 kept")
  {
    const auto read = Load(Drawing(Joined({Open, Stray})));
    REQUIRE(read);
    REQUIRE(read->Document.Entities.size() == 2);
    CHECK(std::get<Insert>(read->Document.Entities.front()).Follows);
    REQUIRE(read->Issues.size()     == 1);
    CHECK(read->Issues.front().What == "INSERT has no SEQEND");
    const auto saved = Tests::Written(read->Document, Options{});
    REQUIRE(saved);
    CHECK(saved->contains(Open));
  }

  TEST_CASE("insert: a SEQEND alone, and no SEQEND at the section's end")
  {
    const auto lone = Load(Drawing(Joined({Open, Closing})));
    REQUIRE(lone);
    CHECK(std::get<Insert>(lone->Document.Entities.front()).Attributes.Closing);
    CHECK(lone->Issues.empty());
    const auto cut = Load(Drawing(Joined({Open, Tagged})));
    REQUIRE(cut);
    CHECK(std::get<Insert>(cut->Document.Entities.front()).Attributes.Children.size() == 1);
    CHECK(cut->Issues.size() == 1);
  }

  TEST_CASE("insert: a record between its attributes ends them, the rest stay")
  {
    const auto read = Load(Drawing(Joined({Open, Tagged, Stray, Tagged, Closing})));
    REQUIRE(read);
    const List &all = read->Document.Entities;
    REQUIRE(all.size() == 4);
    CHECK(std::get<Insert>(all.at(0)).Attributes.Children.size() == 1);
    CHECK(std::holds_alternative<Line>(all.at(1)));
    CHECK(std::get<Unknown>(all.at(2)).Type == "ATTRIB");
    CHECK(std::get<Unknown>(all.at(3)).Type == "SEQEND");
  }

  TEST_CASE("insert: a VERTEX after it stays one, where AutoCAD takes it in")
  {
    std::string vertex{Tagged};
    vertex.replace(vertex.find("ATTRIB"), 6, "VERTEX");
    const auto read = Load(Drawing(Joined({Open, vertex, Closing})));
    REQUIRE(read);
    const List &all = read->Document.Entities;
    REQUIRE(all.size() == 3);
    CHECK(std::get<Insert>(all.at(0)).Attributes.Children.empty());
  }

  TEST_CASE("insert: a new one is its marker, block and point")
  {
    Insert fresh{};
    fresh.Block             = Symbol{"b"};
    fresh.Record.Base.Self  = Handle{0x2a};
    fresh.Record.Base.Owner = Handle{0x1f};
    CHECK(Written(fresh) == Plain);
  }

  TEST_CASE("insert: rows alone pick the array's marker, a plain one none")
  {
    Insert fresh{};
    fresh.Block = Symbol{"b"};
    CHECK(Written(fresh).contains("AcDbBlockReference"));
    fresh.Rows = 2;
    CHECK(Written(fresh).contains("AcDbMInsertBlock"));
    fresh.Rows = 1;
    CHECK(Written(fresh).contains("AcDbBlockReference"));
  }
} // namespace DxfIO::Entities
