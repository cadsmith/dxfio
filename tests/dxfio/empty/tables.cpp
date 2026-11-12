#include "empty/tables.hpp"

#include "../autocad.hpp"
#include "../disk.hpp"
#include "../kind.hpp"
#include "../releases.hpp"
#include "empty/roots.hpp"
#include "tables/shapes/appid.hpp"
#include "tables/shapes/block.hpp"
#include "tables/shapes/dimstyle.hpp"
#include "tables/shapes/layer.hpp"
#include "tables/shapes/linetype.hpp"
#include "tables/shapes/style.hpp"

#include <array>
#include <format>
#include <set>
#include <string>
#include <utility>

namespace DxfIO::Empty
{
  namespace
  {
    struct Made
    {
      Seed     Handles;
      Roots    Ids{Taken(Handles)};
      Document Drawing{};
    };

    template <class KindT>
    void Owned(const DxfIO::Tables::Table<KindT> &one, std::set<Handle> &seen)
    {
      CHECK(seen.insert(one.Head.Base.Self).second);
      for (const KindT &record : one.Records)
      {
        CHECK(record.Record.Base.Owner == one.Head.Base.Self);
        CHECK(seen.insert(record.Record.Base.Self).second);
      }
    }
  } // namespace

  TEST_CASE("tables: a new drawing's, each record with a handle of its own")
  {
    Made fresh;
    Tables(fresh.Drawing, fresh.Ids, fresh.Handles);
    const Document  &drawing = fresh.Drawing;
    std::set<Handle> seen;
    Owned(drawing.Vport, seen);
    Owned(drawing.Ltype, seen);
    Owned(drawing.Layer, seen);
    Owned(drawing.Style, seen);
    Owned(drawing.View, seen);
    Owned(drawing.Ucs, seen);
    Owned(drawing.Appid, seen);
    Owned(drawing.Dimstyle, seen);
    Owned(drawing.BlockRecord, seen);
    CHECK(seen.size() == 20);
    CHECK(drawing.Ltype.Head.Count       == 1);
    CHECK(drawing.BlockRecord.Head.Count == 0);
    CHECK(drawing.Layer.Head.Count       == 1);
    CHECK(drawing.Ltype.Records.front().Record.Base.Self == fresh.Ids.Linetype);
    CHECK(drawing.Layer.Records.front().Style            == fresh.Ids.Normal);
    CHECK(drawing.Layer.Records.front().Material         == fresh.Ids.Materials.Global);
    CHECK(drawing.BlockRecord.Records.back().Record.Base.Self == fresh.Ids.Records.Paper);
    CHECK(drawing.BlockRecord.Records.back().Layout           == fresh.Ids.Layouts.Paper);
  }

  TEST_CASE("tables: a new drawing's text style and ISO-25 as AutoCAD's")
  {
    Made fresh;
    Tables(fresh.Drawing, fresh.Ids, fresh.Handles);
    const auto &style = fresh.Drawing.Style.Records.front();
    CHECK(Written(style)
          == std::format("  0\r\nSTYLE\r\n  5\r\n{}\r\n330\r\n{}\r\n"
                         "100\r\nAcDbSymbolTableRecord\r\n"
                         "100\r\nAcDbTextStyleTableRecord\r\n"
                         "  2\r\nStandard\r\n 70\r\n     0\r\n 40\r\n0.0\r\n"
                         " 41\r\n1.0\r\n 50\r\n0.0\r\n 71\r\n     0\r\n"
                         " 42\r\n2.5\r\n  3\r\narial.ttf\r\n  4\r\n\r\n"
                         "1001\r\nACAD\r\n1000\r\nArial\r\n1071\r\n       34\r\n",
                         Hex(style.Record.Base.Self), Hex(style.Record.Base.Owner)));
    const auto &iso = fresh.Drawing.Dimstyle.Records.back();
    CHECK(Written(iso)
          == std::format("  0\r\nDIMSTYLE\r\n105\r\n{}\r\n330\r\n{}\r\n"
                         "100\r\nAcDbSymbolTableRecord\r\n"
                         "100\r\nAcDbDimStyleTableRecord\r\n"
                         "  2\r\nISO-25\r\n 70\r\n     0\r\n"
                         " 41\r\n2.5\r\n 42\r\n0.625\r\n 43\r\n3.75\r\n"
                         " 44\r\n1.25\r\n 73\r\n     0\r\n 74\r\n     0\r\n"
                         " 77\r\n     1\r\n 78\r\n     8\r\n140\r\n2.5\r\n"
                         "141\r\n2.5\r\n143\r\n0.03937007874016\r\n"
                         "147\r\n0.625\r\n171\r\n     3\r\n172\r\n     1\r\n"
                         "271\r\n     2\r\n272\r\n     2\r\n274\r\n     3\r\n"
                         "278\r\n    44\r\n283\r\n     0\r\n284\r\n     8\r\n"
                         "340\r\n{}\r\n",
                         Hex(iso.Record.Base.Self), Hex(iso.Record.Base.Owner),
                         Hex(fresh.Ids.Style)));
  }

  TEST_CASE("tables: a new drawing's records as AutoCAD writes them, from R2000")
  {
    Made fresh;
    Tables(fresh.Drawing, fresh.Ids, fresh.Handles);
    const Document &drawing = fresh.Drawing;
    for (const Version release : Releases)
    {
      const std::string source = Acad(Fixture("empty", release));
      CAPTURE(release);
      const auto same = [&](const std::string &ours, std::string_view self) -> void
      {
        CAPTURE(self);
        CHECK(Masked(ours) == Masked(Autocad(source, self)));
      };
      const auto &linetypes = drawing.Ltype.Records;
      same(Written(linetypes.at(0), release), "14");
      same(Written(linetypes.at(1), release), "15");
      same(Written(linetypes.at(2), release), "16");
      same(Written(drawing.Layer.Records.front(), release), "10");
      same(Written(drawing.Style.Records.front(), release), "11");
      same(Written(drawing.Appid.Records.front(), release), "12");
      same(Written(drawing.Dimstyle.Records.front(), release), "1B0");
      same(Written(drawing.Dimstyle.Records.back(), release), "27");
      same(Written(drawing.BlockRecord.Records.front(), release), "1F");
      same(Written(drawing.BlockRecord.Records.back(), release), "D2");
    }
  }
} // namespace DxfIO::Empty
