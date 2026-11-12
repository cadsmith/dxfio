#include "drawn.hpp"

#include <cmath>
#include <cstddef>
#include <numbers>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace DxfIO::Down
{
  namespace
  {
    constexpr double Pitch = 5.0 / 3.0;

    constexpr double Degrees = 180 / std::numbers::pi;

    constexpr std::string_view Dropped = "LlOoKk";

    constexpr std::string_view Valued = "ACcFfHQTWp";

    constexpr int Columns = 3;

    constexpr int Top = 3;

    auto Fraction(std::string_view rest, std::string &out) -> std::size_t
    {
      const std::size_t      end = rest.find(';');
      const std::string_view body =
          rest.substr(0, end == std::string_view::npos ? rest.size() : end);
      for (const char one : body)
      {
        out.push_back(one == '^' || one == '#' ? '/' : one);
      }
      return end == std::string_view::npos ? rest.size() : end + 1;
    }

    auto Code(std::string_view rest, std::string &out, std::vector<std::string> &lines)
        -> std::size_t
    {
      if (rest.empty())
      {
        out.push_back('\\');
        return 0;
      }
      const char letter = rest.front();
      if (letter == 'P')
      {
        lines.push_back(std::exchange(out, {}));
        return 1;
      }
      if (letter == '~')
      {
        out.push_back(' ');
        return 1;
      }
      if (letter == 'S')
      {
        return 1 + Fraction(rest.substr(1), out);
      }
      if (Dropped.contains(letter))
      {
        return 1;
      }
      if (Valued.contains(letter))
      {
        const std::size_t end = rest.find(';');
        return end == std::string_view::npos ? rest.size() : end + 1;
      }
      out.push_back(letter);
      return 1;
    }

    auto Dot(const Vector &one, const Vector &other) -> double
    {
      return (one.X * other.X) + (one.Y * other.Y) + (one.Z * other.Z);
    }

    auto Seen(const Vector &one, const Axes &from) -> Vector
    {
      return Vector{.X = Dot(one, from.X), .Y = Dot(one, from.Y), .Z = Dot(one, from.Z)};
    }

    auto Turned(const Entities::Paragraph &one, const Axes &from) -> double
    {
      if (one.Direction)
      {
        return std::atan2(Dot(*one.Direction, from.Y), Dot(*one.Direction, from.X));
      }
      return one.Rotation.value_or(0);
    }
  } // namespace

  auto Plain(std::string_view formatted) -> std::vector<std::string>
  {
    std::vector<std::string> lines;
    std::string              out;
    for (std::size_t at = 0; at < formatted.size();)
    {
      const char one = formatted.at(at);
      if (one == '\\')
      {
        at += 1 + Code(formatted.substr(at + 1), out, lines);
      }
      else if (one == '{' || one == '}')
      {
        ++at;
      }
      else if (one == '\n')
      {
        lines.push_back(std::exchange(out, {}));
        ++at;
      }
      else
      {
        out.push_back(one);
        ++at;
      }
    }
    lines.push_back(std::move(out));
    return lines;
  }

  auto Lettered(const Entities::MText &one, Seed &handles) -> std::vector<Entities::Text>
  {
    const Entities::Paragraph     &body  = *one.Paragraph;
    const std::vector<std::string> lines = Plain(body.Value);
    const int attachment = body.Attachment >= 1 && body.Attachment <= 9 ? body.Attachment - 1 : 0;
    const int row        = attachment / Columns;
    const int column     = attachment % Columns;
    const double pitch   = body.Height * Pitch * body.Factor;
    const Axes   plane   = Arbitrary(body.Extrusion);
    const Vector origin  = Seen(body.Position, plane);
    const double angle   = Turned(body, plane);
    const auto   spread  = static_cast<double>(lines.size() - 1);
    double       shift   = 0.0;
    if (row == 1)
    {
      shift = spread / 2;
    }
    else if (row != 0)
    {
      shift = spread;
    }
    std::vector<Entities::Text> out;
    for (std::size_t index = 0; index < lines.size(); ++index)
    {
      const double down = (static_cast<double>(index) - shift) * pitch;
      const Vector at{
          .X = origin.X + (down * std::sin(angle)),
          .Y = origin.Y - (down * std::cos(angle)),
          .Z = origin.Z,
      };
      Entities::Text text{};
      text.Record = index == 0 ? one.Record : Child(one.Record, handles);
      if (index != 0)
      {
        text.Record.Base.Owner = one.Record.Base.Owner;
      }
      text.Lettering.Position   = at;
      text.Lettering.Alignment  = at;
      text.Lettering.Height     = body.Height;
      text.Lettering.Value      = lines.at(index);
      text.Lettering.Rotation   = angle * Degrees;
      text.Lettering.Style      = body.Style;
      text.Lettering.Extrusion  = body.Extrusion;
      text.Lettering.Horizontal = column;
      text.Vertical             = Top - row;
      out.push_back(std::move(text));
    }
    return out;
  }
} // namespace DxfIO::Down
