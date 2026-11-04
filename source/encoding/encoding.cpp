#include "encoding/encoding.hpp"

#include "encoding/carets.hpp"
#include "encoding/escapes.hpp"
#include "encoding/points.hpp"
#include "unknown/io.hpp"

#include <cstddef>
#include <optional>

namespace DxfIO::Encoding
{
namespace
{
constexpr char32_t Ascii = 0x80;

constexpr char Backslash = '\\';

constexpr char Hat = '^';

using Plain = std::size_t (*)(std::string &out, std::string_view raw, const Codepage *page);

using Beyond = void (*)(std::string &out, char32_t point, const Codepage *page);

auto Kept(std::string &out, std::string_view raw, const Codepage * /*unused*/) -> std::size_t
{
  out.push_back(raw.front());
  return 1;
}

auto Looked(std::string &out, std::string_view raw, const Codepage *page) -> std::size_t
{
  const auto lead = static_cast<unsigned char>(raw.front());
  if (raw.size() > 1)
  {
    const char32_t pair = Paired(*page, lead, static_cast<unsigned char>(raw.at(1)));
    if (pair != 0)
    {
      Put(out, pair);
      return 2;
    }
  }
  Put(out, Decoded(*page, lead));
  return 1;
}

void Spelled(std::string &out, char32_t point, const Codepage * /*unused*/)
{
  Put(out, point);
}

void Mapped(std::string &out, char32_t point, const Codepage *page)
{
  const std::optional<char> byte = Encoded(*page, point);
  if (byte)
  {
    out.push_back(*byte);
    return;
  }
  if (const std::optional<Twin> pair = Doubled(*page, point))
  {
    out.append(pair->data(), pair->size());
    return;
  }
  Escaped(out, point);
}

template <Plain Bytes>
auto Unpacked(std::string_view raw, const Codepage *page) -> std::string
{
  std::string out;
  out.reserve(raw.size());
  while (!raw.empty())
  {
    if (const std::optional<Escape> found = Leading(raw))
    {
      Put(out, found->Point);
      raw.remove_prefix(found->Length);
    }
    else if (const std::optional<char> byte = Caret(raw))
    {
      out.push_back(*byte);
      raw.remove_prefix(2);
    }
    else
    {
      raw.remove_prefix(Bytes(out, raw, page));
    }
  }
  return out;
}

template <Beyond High>
auto Packed(std::string_view text, const Codepage *page) -> std::string
{
  std::string out;
  out.reserve(text.size());
  while (!text.empty())
  {
    if (Leading(text))
    {
      Escaped(out, char32_t{Backslash});
      text.remove_prefix(1);
      continue;
    }
    const Step one = First(text);
    if (one.Point < Ascii)
    {
      Careted(out, static_cast<char>(one.Point));
    }
    else
    {
      High(out, one.Point, page);
    }
    text.remove_prefix(one.Length);
  }
  return out;
}

auto Sound(std::string_view text) -> std::size_t
{
  std::size_t at = 0;
  while (at < text.size())
  {
    const bool ascii = static_cast<unsigned char>(text.at(at)) < Ascii;
    const std::size_t length = ascii ? 1 : First(text.substr(at)).Length;
    if (!ascii && length == 1)
    {
      return at;
    }
    at += length;
  }
  return at;
}

auto Whole(std::string text) -> std::string
{
  const std::size_t sound = Sound(text);
  if (sound == text.size())
  {
    return text;
  }
  const std::string rest = text.substr(sound);
  text.resize(sound);
  for (std::string_view left{rest}; !left.empty();)
  {
    const Step one = First(left);
    Put(text, one.Point);
    left.remove_prefix(one.Length);
  }
  return text;
}

auto Bytewise(std::string_view raw, const Codepage * /*unused*/) -> std::string
{
  std::string out;
  out.reserve(raw.size());
  for (const char byte : raw)
  {
    Put(out, Decoded(Standard(), static_cast<unsigned char>(byte)));
  }
  return out;
}

auto Backward(std::string_view text, const Codepage * /*unused*/) -> std::string
{
  std::string out;
  out.reserve(text.size());
  while (!text.empty())
  {
    const Step one = First(text);
    if (one.Point >= Ascii)
    {
      Mapped(out, one.Point, &Standard());
    }
    else if (one.Point == Hat)
    {
      out.push_back(Hat);
    }
    else
    {
      Careted(out, static_cast<char>(one.Point));
    }
    text.remove_prefix(one.Length);
  }
  return out;
}

auto Checked(std::string_view raw, const Codepage *page) -> std::string
{
  return Whole(Unpacked<Kept>(raw, page));
}
} // namespace

auto Utf8() -> Scheme
{
  return Scheme{.Page = nullptr, .In = Checked, .Out = Packed<Spelled>};
}

auto Paged(const Codepage &page) -> Scheme
{
  return Scheme{.Page = &page, .In = Unpacked<Looked>, .Out = Packed<Mapped>};
}

auto Opaque() -> Scheme
{
  return Scheme{.Page = nullptr, .In = Bytewise, .Out = Backward};
}

auto Of(Version release, const Codepage &page) -> Scheme
{
  return release >= Version::R2007 ? Utf8() : Paged(page);
}

auto Decode(const Scheme &with, std::string_view raw) -> std::string
{
  return with.In(raw, with.Page);
}

auto Encode(const Scheme &with, std::string_view text) -> std::string
{
  return with.Out(text, with.Page);
}
} // namespace DxfIO::Encoding
