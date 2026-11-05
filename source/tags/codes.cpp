#include "tags/codes.hpp"

#include "seek.hpp"

#include <algorithm>
#include <array>
#include <utility>

namespace DxfIO::Tags::Codes
{
namespace
{
struct Range
{
  int First{};
  int Last{};
  Type Is{};
};

constexpr std::array Ranges{
    Range{.First = 5, .Last = 5, .Is = Type::Handle},       Range{.First = 105, .Last = 105, .Is = Type::Handle},
    Range{.First = 320, .Last = 369, .Is = Type::Handle},   Range{.First = 390, .Last = 399, .Is = Type::Handle},
    Range{.First = 480, .Last = 481, .Is = Type::Handle},   Range{.First = 310, .Last = 319, .Is = Type::Binary},
    Range{.First = 1004, .Last = 1004, .Is = Type::Binary}, Range{.First = 10, .Last = 59, .Is = Type::Real},
    Range{.First = 110, .Last = 149, .Is = Type::Real},     Range{.First = 210, .Last = 239, .Is = Type::Real},
    Range{.First = 460, .Last = 469, .Is = Type::Real},     Range{.First = 1010, .Last = 1059, .Is = Type::Real},
    Range{.First = 60, .Last = 79, .Is = Type::Int16},      Range{.First = 170, .Last = 179, .Is = Type::Int16},
    Range{.First = 270, .Last = 289, .Is = Type::Int16},    Range{.First = 370, .Last = 389, .Is = Type::Int16},
    Range{.First = 400, .Last = 409, .Is = Type::Int16},    Range{.First = 1060, .Last = 1070, .Is = Type::Int16},
    Range{.First = 90, .Last = 99, .Is = Type::Int32},      Range{.First = 420, .Last = 429, .Is = Type::Int32},
    Range{.First = 440, .Last = 459, .Is = Type::Int32},    Range{.First = 1071, .Last = 1071, .Is = Type::Int32},
    Range{.First = 160, .Last = 169, .Is = Type::Int64},    Range{.First = 290, .Last = 299, .Is = Type::Boolean},
};
} // namespace

// The codes the DXF reference defines, as the ranges of its table of group code value types.
constexpr std::array<std::pair<int, int>, 27> Given{{
    {0, 59},    {60, 79},   {90, 99},   {100, 102}, {105, 105}, {110, 149},   {160, 169},   {170, 179},   {210, 239},
    {270, 309}, {310, 369}, {370, 389}, {390, 399}, {400, 409}, {410, 419},   {420, 429},   {430, 439},   {440, 449},
    {450, 459}, {460, 469}, {470, 479}, {480, 481}, {999, 999}, {1000, 1009}, {1010, 1059}, {1060, 1070}, {1071, 1071},
}};

auto Defined(int code) -> bool
{
  return std::ranges::any_of(Given, [code](const std::pair<int, int> &range) -> bool
                             { return code >= range.first && code <= range.second; });
}

auto Of(int code) -> Type
{
  const auto *found =
      SeekIf(Ranges, [code](const Range &one) -> bool { return code >= one.First && code <= one.Last; });
  return found == nullptr ? Type::Text : found->Is;
}
} // namespace DxfIO::Tags::Codes
