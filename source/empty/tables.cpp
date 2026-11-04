#include "tables.hpp"

#include "dxfio/group.hpp"
#include "dxfio/vector.hpp"
#include "dxfio/xdata.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace DxfIO::Empty
{
namespace
{
constexpr Vector Sheet{.X = 420, .Y = 297, .Z = 0};

constexpr std::int64_t Pitch = 34;

constexpr int Typeface = 1000;

constexpr int Family = 1071;

constexpr int Named = 100;

constexpr int Count = 71;

constexpr int Entry = 340;

template <class KindT>
auto Called(std::string name, Handle self = {}) -> KindT
{
  KindT out{};
  out.Record.Name = std::move(name);
  out.Record.Base.Self = self;
  return out;
}

auto Builtin(const std::string &name) -> bool
{
  return name == "ByBlock" || name == "ByLayer" || name == "*Model_Space" || name == "*Paper_Space";
}

template <class KindT>
auto Tabled(std::string name, std::vector<KindT> records, Seed &handles) -> DxfIO::Tables::Table<KindT>
{
  DxfIO::Tables::Table<KindT> out{};
  out.Head.Base.Self = handles.Take();
  out.Head.Name = std::move(name);
  out.Head.Count = static_cast<int>(
      std::ranges::count_if(records, [](const KindT &one) -> auto { return !Builtin(one.Record.Name); }));
  out.Records = std::move(records);
  for (KindT &one : out.Records)
  {
    DxfIO::Base &base = one.Record.Base;
    base.Self = base.Self ? base.Self : handles.Take();
    base.Owner = out.Head.Base.Self;
  }
  return out;
}

auto Active() -> DxfIO::Tables::Vport
{
  auto out = Called<DxfIO::Tables::Vport>("*Active");
  out.Center = Vector{.X = Sheet.X / 2, .Y = Sheet.Y / 2, .Z = 0};
  out.Height = Sheet.Y;
  out.Aspect = Sheet.X / Sheet.Y;
  return out;
}

auto Linetypes(const Roots &ids) -> std::vector<DxfIO::Tables::Ltype>
{
  auto continuous = Called<DxfIO::Tables::Ltype>("Continuous");
  continuous.Description = "Solid line";
  return {
      Called<DxfIO::Tables::Ltype>("ByBlock", ids.Linetype),
      Called<DxfIO::Tables::Ltype>("ByLayer"),
      continuous,
  };
}

auto Zero(const Roots &ids) -> DxfIO::Tables::Layer
{
  auto out = Called<DxfIO::Tables::Layer>("0");
  out.Style = ids.Normal;
  out.Material = ids.Materials.Global;
  return out;
}

auto Standard(const Roots &ids) -> DxfIO::Tables::Style
{
  auto out = Called<DxfIO::Tables::Style>("Standard", ids.Style);
  out.Font = "arial.ttf";
  out.Record.Base.Extended.push_back(Xdata{
      .Application = "ACAD",
      .Groups =
          {
              Group{.Code = Typeface, .Value = std::string{"Arial"}},
              Group{.Code = Family, .Value = Pitch},
          },
  });
  return out;
}

auto Metric(const Roots &ids) -> DxfIO::Tables::Dimstyle
{
  const DxfIO::Tables::Dimstyle iso{
      .Dimasz = 2.5,
      .Dimexo = 0.625,
      .Dimdli = 3.75,
      .Dimexe = 1.25,
      .Dimtih = 0,
      .Dimtoh = 0,
      .Dimtad = 1,
      .Dimzin = 8,
      .Dimtxt = 2.5,
      .Dimcen = 2.5,
      .Dimaltf = 0.03937007874016,
      .Dimgap = 0.625,
      .Dimaltd = 3,
      .Dimtofl = 1,
      .Dimdec = 2,
      .Dimtdec = 2,
      .Dimalttd = 3,
      .Dimdsep = ',',
      .Dimtolj = 0,
      .Dimtzin = 8,
  };
  auto out = iso;
  out.Record.Name = "ISO-25";
  out.Dimtxsty = ids.Style;
  return out;
}

auto Dimstyles(const Roots &ids, Seed &handles) -> DxfIO::Tables::Table<DxfIO::Tables::Dimstyle>
{
  auto imperial = Called<DxfIO::Tables::Dimstyle>("Standard");
  imperial.Dimtxsty = ids.Style;
  auto out = Tabled("DIMSTYLE", std::vector{imperial, Metric(ids)}, handles);
  out.Head.Base.Extra = {
      Group{.Code = Named, .Value = std::string{"AcDbDimStyleTable"}},
      Group{.Code = Count, .Value = std::int64_t{1}},
      Group{.Code = Entry, .Value = out.Records.back().Record.Base.Self},
  };
  return out;
}

auto Records(const Roots &ids) -> std::vector<DxfIO::Tables::BlockRecord>
{
  auto model = Called<DxfIO::Tables::BlockRecord>("*Model_Space", ids.Records.Model);
  model.Layout = ids.Layouts.Model;
  auto paper = Called<DxfIO::Tables::BlockRecord>("*Paper_Space", ids.Records.Paper);
  paper.Layout = ids.Layouts.Paper;
  return {model, paper};
}
} // namespace

void Tables(Document &drawing, const Roots &ids, Seed &handles)
{
  drawing.Vport = Tabled("VPORT", std::vector{Active()}, handles);
  drawing.Ltype = Tabled("LTYPE", Linetypes(ids), handles);
  drawing.Layer = Tabled("LAYER", std::vector{Zero(ids)}, handles);
  drawing.Style = Tabled("STYLE", std::vector{Standard(ids)}, handles);
  drawing.View = Tabled<DxfIO::Tables::View>("VIEW", {}, handles);
  drawing.Ucs = Tabled<DxfIO::Tables::Ucs>("UCS", {}, handles);
  drawing.Appid = Tabled("APPID", std::vector{Called<DxfIO::Tables::Appid>("ACAD")}, handles);
  drawing.Dimstyle = Dimstyles(ids, handles);
  drawing.BlockRecord = Tabled("BLOCK_RECORD", Records(ids), handles);
}
} // namespace DxfIO::Empty
