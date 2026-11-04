#include "empty/classes.hpp"

#include "../autocad.hpp"
#include "../disk.hpp"
#include "../fields/fixture.hpp"
#include "../releases.hpp"
#include "empty/objects.hpp"
#include "empty/roots.hpp"
#include "header/io.hpp"
#include "raw/io.hpp"

#include <doctest/doctest.h>

#include <array>
#include <string>
#include <utility>
#include <vector>

namespace DxfIO::Empty
{
namespace
{
auto Made() -> std::vector<Object>
{
  Seed handles;
  const Roots ids = Taken(handles);
  return Objects(ids, Handle{.Value = 0x99}, handles);
}

auto Written(const std::vector<Raw> &all, Version release) -> std::string
{
  Fields::Sheet page{release};
  REQUIRE(Put(page.To, all));
  return page.Out;
}
} // namespace

TEST_CASE("classes: R2018's, each with how many objects are of its kind")
{
  const std::vector<Raw> all = Classes(Version::R2018, Made());
  REQUIRE(all.size() == 5);
  CHECK(Written({all.front(), all.at(3)}, Version::R2018) ==
        "  0\r\nCLASS\r\n  1\r\nACDBDICTIONARYWDFLT\r\n"
        "  2\r\nAcDbDictionaryWithDefault\r\n  3\r\nObjectDBX Classes\r\n"
        " 90\r\n        0\r\n 91\r\n        1\r\n"
        "280\r\n     0\r\n281\r\n     0\r\n"
        "  0\r\nCLASS\r\n  1\r\nSCALE\r\n  2\r\nAcDbScale\r\n"
        "  3\r\nObjectDBX Classes\r\n 90\r\n     1153\r\n"
        " 91\r\n       17\r\n280\r\n     0\r\n281\r\n     0\r\n");
}

TEST_CASE("classes: R2000's, the placeholder and the layout too, uncounted")
{
  const std::vector<Raw> all = Classes(Version::R2000, Made());
  REQUIRE(all.size() == 7);
  CHECK(Written({all.at(2)}, Version::R2000) == "  0\r\nCLASS\r\n  1\r\nLAYOUT\r\n  2\r\nAcDbLayout\r\n"
                                                "  3\r\nObjectDBX Classes\r\n 90\r\n        0\r\n"
                                                "280\r\n     0\r\n281\r\n     0\r\n");
}

TEST_CASE("classes: each as AutoCAD declares it, but for its count")
{
  constexpr std::array chosen{Version::R2000, Version::R2004, Version::R2018};
  const std::vector<Object> objects = Made();
  for (const Version release : chosen)
  {
    const std::string source = Acad(Fixture("empty", release));
    for (const Raw &one : Classes(release, objects))
    {
      const auto &name = std::get<std::string>(one.Groups.front().Value);
      CAPTURE(release);
      CAPTURE(name);
      Raw uncounted = one;
      std::erase_if(uncounted.Groups, [](const Group &held) -> bool { return held.Code == 91; });
      std::string theirs = Declared(source, name);
      const std::size_t at = theirs.find(" 91\r\n");
      if (at != std::string::npos)
      {
        theirs.erase(at, theirs.find("\r\n", at + 5) + 2 - at);
      }
      CHECK(Written({uncounted}, release) == theirs);
    }
  }
}

TEST_CASE("classes: none for a kind no object is of")
{
  CHECK(Classes(Version::R2018, {}).empty());
}
} // namespace DxfIO::Empty
