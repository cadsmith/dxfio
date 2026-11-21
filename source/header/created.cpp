#include "header/created.hpp"

#include "fields/points.hpp"
#include "header/rows.hpp"
#include "match.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

namespace DxfIO::Header
{
  namespace
  {
    auto Point(const Row &one, const Vector &held) -> std::vector<Tag>
    {
      std::vector<Tag> out{
          Tag{.Code = one.Code               , .Value = held.X},
          Tag{.Code = one.Code + Fields::Axis, .Value = held.Y},
      };
      if (!one.Flat)
      {
        out.push_back(Tag{.Code = one.Code + (2 * Fields::Axis), .Value = held.Z});
      }
      return out;
    }

    auto Groups(const Row &one) -> std::vector<Tag>
    {
      return std::visit(
          DxfIO::Match{
              [&one](const Vector &held) -> std::vector<Tag> { return Point(one, held); },
              [&one](std::string_view held) -> std::vector<DxfIO::Tag>
              {
                return std::vector{
                    Tag{.Code = one.Code, .Value = std::string{held}},
                };
              },
              [&one](const auto &held) -> auto
              { return std::vector{Tag{.Code = one.Code, .Value = held}}; },
          },
          one.Value);
    }
  } // namespace

  auto Created(Version release) -> std::vector<Variable>
  {
    std::vector<Variable> out;
    for (const Row &one : Rows::All())
    {
      if (one.Since <= release && release <= one.Until)
      {
        out.push_back(Variable{.Name = std::string{one.Name}, .Values = Groups(one)});
      }
    }
    return out;
  }
} // namespace DxfIO::Header
