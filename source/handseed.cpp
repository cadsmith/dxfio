#include "handseed.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <variant>

namespace DxfIO
{
namespace
{
constexpr std::string_view Variable = "$HANDSEED";
constexpr int Code = 5;
} // namespace

auto ReadSeed(const Document &drawing) -> Handle
{
  const auto found = std::ranges::find(drawing.Header, Variable, &Header::Variable::Name);
  if (found == drawing.Header.end() || found->Values.empty())
  {
    return {};
  }
  const auto *held = std::get_if<Handle>(&found->Values.front().Value);
  return held == nullptr ? Handle{} : *held;
}

void WriteSeed(Document &drawing, Handle next)
{
  const Group value{.Code = Code, .Value = next};
  const auto found = std::ranges::find(drawing.Header, Variable, &Header::Variable::Name);
  if (found == drawing.Header.end())
  {
    drawing.Header.push_back(Header::Variable{.Name = std::string{Variable}, .Values = {value}});
    return;
  }
  found->Values.assign(1, value);
}
} // namespace DxfIO
