#include "plan/foreign.hpp"

#include "objects/io.hpp"

#include <doctest/doctest.h>

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace DxfIO::Plan
{
namespace
{
constexpr Handle Kept{0x10};

constexpr Handle Gone{0x11};

auto Drawn() -> std::vector<Object>
{
  Unknown style{};
  style.Type = Symbol{"VISUALSTYLE"};
  style.Base.Self = Gone;
  Objects::Scale scale{};
  scale.Base.Self = Kept;
  Objects::Dictionary names{};
  names.Entries = {Objects::Entry{.Name = "Kept", .Soft = Kept, .Hard = {}},
                   Objects::Entry{.Name = "Gone", .Soft = Gone, .Hard = {}}};
  return {Object{names}, Object{scale}, Object{style}};
}
} // namespace

TEST_CASE("foreign: what no kind types stays behind, its entry with it")
{
  const std::vector<Object> out = Foreign(Drawn());
  REQUIRE(out.size() == 2);
  const auto *names = std::get_if<Objects::Dictionary>(&out.front());
  REQUIRE(names != nullptr);
  REQUIRE(names->Entries.size() == 1);
  CHECK(names->Entries.front().Name == "Kept");
}

TEST_CASE("foreign: an entry held hard goes the same way")
{
  std::vector<Object> all = Drawn();
  std::get<Objects::Dictionary>(all.front()).Entries.at(1) = Objects::Entry{.Name = "Gone", .Soft = {}, .Hard = Gone};
  const std::vector<Object> out = Foreign(std::move(all));
  CHECK(std::get<Objects::Dictionary>(out.front()).Entries.size() == 1);
}

TEST_CASE("foreign: an unknown object with no handle takes no entry with it")
{
  std::vector<Object> all = Drawn();
  all.emplace_back(Unknown{.Type = Symbol{"FOO"}, .Base = {}, .Groups = {}});
  const std::vector<Object> out = Foreign(std::move(all));
  REQUIRE(out.size() == 2);
  const auto &names = std::get<Objects::Dictionary>(out.front());
  REQUIRE(names.Entries.size() == 1);
  CHECK(names.Entries.front().Name == "Kept");
}

TEST_CASE("foreign: objects with nothing foreign come back the same")
{
  std::vector<Object> all = Drawn();
  all.pop_back();
  std::get<Objects::Dictionary>(all.front()).Entries.pop_back();
  CHECK(Foreign(all) == all);
}
} // namespace DxfIO::Plan
