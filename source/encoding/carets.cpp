#include "encoding/carets.hpp"

namespace DxfIO::Encoding
{
namespace
{
constexpr char Mark = '^';

constexpr char Blank = ' ';

constexpr char Offset = '@';

constexpr char Last = '_';

constexpr unsigned char Controls = 0x20;
} // namespace

auto Caret(std::string_view raw) -> std::optional<char>
{
  if (raw.size() < 2 || raw.front() != Mark)
  {
    return std::nullopt;
  }
  const char next = raw.at(1);
  if (next == Blank)
  {
    return Mark;
  }
  if (next >= Offset && next <= Last)
  {
    return static_cast<char>(next - Offset);
  }
  return std::nullopt;
}

void Careted(std::string &out, char letter)
{
  if (letter == Mark)
  {
    out.push_back(Mark);
    out.push_back(Blank);
    return;
  }
  if (static_cast<unsigned char>(letter) < Controls)
  {
    out.push_back(Mark);
    out.push_back(static_cast<char>(letter + Offset));
    return;
  }
  out.push_back(letter);
}
} // namespace DxfIO::Encoding
