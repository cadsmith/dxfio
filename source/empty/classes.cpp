#include "classes.hpp"

#include "objects/io.hpp"
#include "seek.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace DxfIO::Empty
{
namespace
{
constexpr std::string_view Database = "ObjectDBX Classes";

constexpr std::string_view Raster = "ISM";

constexpr std::string_view Wipeout =
    "WipeOut|Product Desc: Object Enabler for WipeOut entity | Company: Autodesk, Inc. | WEB Address: www.autodesk.com";

constexpr std::string_view Wiped = "\"WipeOut|Product Desc:     WipeOut Dbx Application|Company:          Autodesk, "
                                   "Inc.|WEB Address:      www.autodesk.com\"";

constexpr std::int64_t Erasable = 1025;

constexpr std::int64_t Transformable = 1153;

constexpr std::int64_t Everything = 4095;

constexpr std::int64_t Imaged = 2175;

// Every kind AutoCAD writes a CLASS for, as its own files declare it, release by release.
constexpr std::array Kinds{
    Class{.Name = "ACAD_TABLE", .Program = "AcDbTable", .Application = Database, .Flags = Erasable, .Entity = true},
    Class{.Name = "ARC_DIMENSION",
          .Program = "AcDbArcDimension",
          .Application = Database,
          .Flags = Erasable,
          .Entity = true},
    Class{.Name = "DGNUNDERLAY",
          .Program = "AcDbDgnReference",
          .Application = Database,
          .Flags = Transformable,
          .Entity = true},
    Class{.Name = "DWFUNDERLAY",
          .Program = "AcDbDwfReference",
          .Application = Database,
          .Flags = Transformable,
          .Entity = true},
    Class{.Name = "EXTRUDEDSURFACE", .Program = "AcDbExtrudedSurface", .Application = Database, .Entity = true},
    Class{.Name = "HELIX", .Program = "AcDbHelix", .Application = Database, .Flags = Everything, .Entity = true},
    Class{.Name = "IMAGE", .Program = "AcDbRasterImage", .Application = Raster, .Flags = Imaged, .Entity = true},
    Class{.Name = "LARGE_RADIAL_DIMENSION",
          .Program = "AcDbRadialDimensionLarge",
          .Application = Database,
          .Flags = Erasable,
          .Entity = true},
    Class{.Name = "LIGHT", .Program = "AcDbLight", .Application = "SCENEOE", .Flags = Erasable, .Entity = true},
    Class{.Name = "LOFTEDSURFACE", .Program = "AcDbLoftedSurface", .Application = Database, .Entity = true},
    Class{.Name = "MESH", .Program = "AcDbSubDMesh", .Application = Database, .Entity = true},
    Class{.Name = "MULTILEADER",
          .Program = "AcDbMLeader",
          .Application = "ACDB_MLEADER_CLASS",
          .Flags = Erasable,
          .Entity = true},
    Class{.Name = "NURBSURFACE", .Program = "AcDbNurbSurface", .Application = Database, .Entity = true},
    Class{.Name = "PDFUNDERLAY",
          .Program = "AcDbPdfReference",
          .Application = Database,
          .Flags = Everything,
          .Entity = true},
    Class{.Name = "PLANESURFACE",
          .Program = "AcDbPlaneSurface",
          .Application = Database,
          .Flags = Everything,
          .Entity = true},
    Class{.Name = "REVOLVEDSURFACE", .Program = "AcDbRevolvedSurface", .Application = Database, .Entity = true},
    Class{
        .Name = "SECTIONOBJECT", .Program = "AcDbSection", .Application = Database, .Flags = Erasable, .Entity = true},
    Class{.Name = "SURFACE",
          .Program = "AcDbSurface",
          .Application = Database,
          .Flags = Everything,
          .Entity = true,
          .Until = Version::R2010},
    Class{.Name = "SWEPTSURFACE", .Program = "AcDbSweptSurface", .Application = Database, .Entity = true},
    Class{.Name = "WIPEOUT", .Program = "AcDbWipeout", .Application = Wipeout, .Flags = Imaged, .Entity = true},
    Class{.Name = "ACDBDATATABLE", .Program = "AcDbDataTable", .Application = Database, .Until = Version::R2000},
    Class{.Name = "ACDBDICTIONARYWDFLT", .Program = "AcDbDictionaryWithDefault", .Application = Database},
    Class{.Name = "ACDBPLACEHOLDER", .Program = "AcDbPlaceHolder", .Application = Database, .Until = Version::R2000},
    Class{.Name = "DATATABLE", .Program = "AcDbDataTable", .Application = Database, .Since = Version::R2004},
    Class{.Name = "DGNDEFINITION", .Program = "AcDbDgnDefinition", .Application = Database, .Flags = Transformable},
    Class{.Name = "DICTIONARYVAR", .Program = "AcDbDictionaryVar", .Application = Database},
    Class{.Name = "DWFDEFINITION", .Program = "AcDbDwfDefinition", .Application = Database, .Flags = Transformable},
    Class{.Name = "IMAGEDEF", .Program = "AcDbRasterImageDef", .Application = Raster},
    Class{.Name = "IMAGEDEF_REACTOR", .Program = "AcDbRasterImageDefReactor", .Application = Raster, .Flags = 1},
    Class{.Name = "LAYOUT", .Program = "AcDbLayout", .Application = Database, .Until = Version::R2000},
    Class{.Name = "MATERIAL", .Program = "AcDbMaterial", .Application = Database, .Flags = Transformable},
    Class{.Name = "MLEADERSTYLE",
          .Program = "AcDbMLeaderStyle",
          .Application = "ACDB_MLEADERSTYLE_CLASS",
          .Flags = Everything},
    Class{.Name = "PDFDEFINITION", .Program = "AcDbPdfDefinition", .Application = Database, .Flags = Transformable},
    Class{.Name = "RASTERVARIABLES", .Program = "AcDbRasterVariables", .Application = Raster},
    Class{.Name = "SCALE", .Program = "AcDbScale", .Application = Database, .Flags = Transformable},
    Class{.Name = "SORTENTSTABLE", .Program = "AcDbSortentsTable", .Application = Database},
    Class{.Name = "TABLESTYLE", .Program = "AcDbTableStyle", .Application = Database, .Flags = Everything},
    Class{.Name = "VISUALSTYLE", .Program = "AcDbVisualStyle", .Application = Database, .Flags = Everything},
    Class{.Name = "WIPEOUTVARIABLES", .Program = "AcDbWipeoutVariables", .Application = Wiped},
};

constexpr int Named = 1;
constexpr int Program = 2;
constexpr int Application = 3;
constexpr int Proxy = 90;
constexpr int Instances = 91;
constexpr int Proxied = 280;
constexpr int Entity = 281;
} // namespace

auto Declared(const Class &one, Version release, std::int64_t count) -> Raw
{
  Raw out{.Type = "CLASS", .Groups = {}};
  out.Groups = {
      Group{.Code = Named, .Value = std::string{one.Name}},
      Group{.Code = Program, .Value = std::string{one.Program}},
      Group{.Code = Application, .Value = std::string{one.Application}},
      Group{.Code = Proxy, .Value = one.Flags},
  };
  if (release >= Version::R2004)
  {
    out.Groups.push_back(Group{.Code = Instances, .Value = count});
  }
  out.Groups.push_back(Group{.Code = Proxied, .Value = std::int64_t{}});
  out.Groups.push_back(Group{.Code = Entity, .Value = std::int64_t{one.Entity ? 1 : 0}});
  return out;
}

auto Known(std::string_view name, Version release) -> const Class *
{
  const auto *found = Seek(Kinds, name, &Class::Name);
  return found == nullptr || release < found->Since || release > found->Until ? nullptr : found;
}

auto Classes(Version release, std::span<const Object> all) -> std::vector<Raw>
{
  std::vector<Raw> out;
  for (const Class &one : Kinds)
  {
    const auto count =
        std::ranges::count(all, one.Name, [](const Object &held) -> std::string_view { return Objects::Type(held); });
    if (count > 0 && Known(one.Name, release) != nullptr)
    {
      out.push_back(Declared(one, release, static_cast<std::int64_t>(count)));
    }
  }
  return out;
}
} // namespace DxfIO::Empty
