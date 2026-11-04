#include "classes.hpp"

#include "objects/io.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace DxfIO::Empty
{
namespace
{
struct Kind
{
  std::string_view Name;
  std::string_view Program;
  std::string_view Application;
  std::int64_t Flags{};
  Version Until{Version::R2018};
};

constexpr std::string_view Database = "ObjectDBX Classes";

constexpr std::array Kinds{
    Kind{
        .Name = "ACDBDICTIONARYWDFLT",
        .Program = "AcDbDictionaryWithDefault",
        .Application = Database,
    },
    Kind{
        .Name = "ACDBPLACEHOLDER",
        .Program = "AcDbPlaceHolder",
        .Application = Database,
        .Until = Version::R2000,
    },
    Kind{
        .Name = "LAYOUT",
        .Program = "AcDbLayout",
        .Application = Database,
        .Until = Version::R2000,
    },
    Kind{
        .Name = "TABLESTYLE",
        .Program = "AcDbTableStyle",
        .Application = Database,
        .Flags = 4095,
    },
    Kind{
        .Name = "MATERIAL",
        .Program = "AcDbMaterial",
        .Application = Database,
        .Flags = 1153,
    },
    Kind{
        .Name = "SCALE",
        .Program = "AcDbScale",
        .Application = Database,
        .Flags = 1153,
    },
    Kind{
        .Name = "MLEADERSTYLE",
        .Program = "AcDbMLeaderStyle",
        .Application = "ACDB_MLEADERSTYLE_CLASS",
        .Flags = 4095,
    },
};

constexpr int Named = 1;
constexpr int Program = 2;
constexpr int Application = 3;
constexpr int Proxy = 90;
constexpr int Instances = 91;
constexpr int Proxied = 280;
constexpr int Entity = 281;

auto Declared(const Kind &one, Version release, std::int64_t count) -> Raw
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
  out.Groups.push_back(Group{.Code = Entity, .Value = std::int64_t{}});
  return out;
}
} // namespace

auto Classes(Version release, std::span<const Object> all) -> std::vector<Raw>
{
  std::vector<Raw> out;
  for (const Kind &one : Kinds)
  {
    const auto count =
        std::ranges::count(all, one.Name, [](const Object &held) -> std::string_view { return Objects::Type(held); });
    if (count > 0 && release <= one.Until)
    {
      out.push_back(Declared(one, release, static_cast<std::int64_t>(count)));
    }
  }
  return out;
}
} // namespace DxfIO::Empty
