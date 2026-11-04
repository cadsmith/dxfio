#include "file/file.hpp"

#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string_view>
#include <vector>

namespace DxfIO::File
{
namespace
{
auto Framed(std::string_view text, std::vector<Issue> *issues = nullptr) -> std::expected<std::vector<Section>, Failure>
{
  const auto all = Tags::All(text);
  REQUIRE(all);
  return Read(*all, issues);
}
} // namespace

TEST_CASE("file: the sections in their order, each its groups between its name and ENDSEC")
{
  const auto all = Tags::All("  0\nSECTION\n  2\nHEADER\n  9\n$ACADVER\n"
                             "  1\nAC1032\n  0\nENDSEC\n"
                             "  0\nSECTION\n  2\nACDSDATA\n 70\n2\n"
                             "  0\nENDSEC\n"
                             "  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n"
                             "  0\nENDSEC\n  0\nEOF\n  0\nafter\n");
  REQUIRE(all);
  const auto sections = Read(*all);
  REQUIRE(sections);
  REQUIRE(sections->size() == 3);
  CHECK(sections->at(0).Name == "HEADER");
  CHECK(sections->at(0).Body.size() == 2);
  CHECK(sections->at(1).Name == "ACDSDATA");
  CHECK(sections->at(1).Body.size() == 1);
  CHECK(sections->at(2).Body.size() == 1);
}

TEST_CASE("file: what stands outside a section is a failure")
{
  CHECK(Framed("  0\nLINE\n") == std::unexpected{Failure{1, "SECTION expected, found group 0"}});
  CHECK(Framed("  0\nSECTION\n  0\nENDSEC\n") == std::unexpected{Failure{1, "section without a name"}});
  CHECK(Framed("  0\nSECTION\n") == std::unexpected{Failure{1, "section without a name"}});
}

TEST_CASE("file: the last section cut before its ENDSEC closes at the end of the file, told")
{
  std::vector<Issue> issues;
  const auto sections = Framed("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n", &issues);
  REQUIRE(sections);
  REQUIRE(sections->size() == 1);
  CHECK(sections->front().Name == "ENTITIES");
  CHECK(sections->front().Body.size() == 1);
  CHECK(issues == std::vector{Issue{.Line = 1, .What = "ENDSEC of ENTITIES missing, closed at the end of the file"}});
  CHECK(Framed("  0\nSECTION\n  2\nENTITIES\n  0\nLINE\n")->size() == 1);
}

TEST_CASE("file: a TABLE outside a TABLES section opens one that runs to the next section, told")
{
  std::vector<Issue> issues;
  const auto sections = Framed("  0\nSECTION\n  2\nHEADER\n  0\nENDSEC\n"
                               "  0\nTABLE\n  2\nLAYER\n 70\n1\n  0\nENDTAB\n"
                               "  0\nTABLE\n  2\nSTYLE\n  0\nENDTAB\n"
                               "  0\nSECTION\n  2\nENTITIES\n  0\nENDSEC\n  0\nEOF\n",
                               &issues);
  REQUIRE(sections);
  REQUIRE(sections->size() == 3);
  CHECK(sections->at(1).Name == "TABLES");
  CHECK(sections->at(1).Body.size() == 7);
  CHECK(sections->at(2).Name == "ENTITIES");
  CHECK(issues == std::vector{Issue{.Line = 7, .What = "TABLE outside a TABLES section, taken as one"}});
  const auto last = Framed("  0\nTABLE\n  2\nLAYER\n  0\nENDTAB\n  0\nEOF\n");
  REQUIRE(last);
  REQUIRE(last->size() == 1);
  CHECK(last->front().Body.size() == 3);
}

TEST_CASE("file: a file without its EOF ends with its last section")
{
  const auto sections = Framed("  0\nSECTION\n  2\nENTITIES\n  0\nENDSEC\n");
  REQUIRE(sections);
  REQUIRE(sections->size() == 1);
  CHECK(sections->front().Body.empty());
}
} // namespace DxfIO::File
