#include "fields/owned.hpp"

#include "bytes.hpp"
#include "chained.hpp"
#include "fields/text.hpp"
#include "hex.hpp"
#include "match.hpp"
#include "tags/codes.hpp"

#include "dxfio/handle.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace DxfIO::Fields
{
  namespace
  {
    constexpr std::size_t Pair = 2;

    constexpr int Start = 0;

    auto Unhexed(std::vector<std::byte> &out, std::string_view text) -> bool
    {
      if (text.size() % Pair != 0)
      {
        return false;
      }
      const std::size_t kept = out.size();
      for (std::size_t at = 0; at < text.size(); at += Pair)
      {
        const std::optional<unsigned int> byte = Unhex(text.substr(at, Pair));
        if (!byte)
        {
          out.resize(kept);
          return false;
        }
        out.push_back(static_cast<std::byte>(*byte));
      }
      return true;
    }

    auto Verbatim(int code) -> bool
    {
      const Tags::Codes::Type is = Tags::Codes::Of(code);
      return code == Start
          || code == Tags::Codes::Control
          || code == Tags::Codes::Marker
          || is   == Tags::Codes::Type::Handle
          || is   == Tags::Codes::Type::Binary;
    }

    auto Word(int code, std::string_view raw) -> Content
    {
      const Tags::Codes::Type is = Tags::Codes::Of(code);
      if (is == Tags::Codes::Type::Handle)
      {
        if (const std::optional<Handle> found = ParseHandle(raw))
        {
          return *found;
        }
      }
      if (is == Tags::Codes::Type::Binary)
      {
        std::vector<std::byte> bytes;
        bytes.reserve(raw.size() / Pair);
        if (Unhexed(bytes, raw))
        {
          return Chunk{std::move(bytes)};
        }
      }
      return std::string{raw};
    }

    auto Text(int code, std::string_view raw, const Encoding::Scheme &with) -> Content
    {
      return Verbatim(code) ? Word(code, raw) : Encoding::Decode(with, raw);
    }
  } // namespace

  auto Owned(const Tags::Tag &one, const Encoding::Scheme &with) -> Tag
  {
    return Tag{
        .Code  = one.Code,
        .Value = std::visit(
            DxfIO::Match{
                [&one, &with](std::string_view raw) -> Content
                { return Text(one.Code, raw, with); },
                [](const Tags::Chunk &bytes) -> Content
                {
                  Chunk out;
                  Append(out.Bytes, bytes.Bytes);
                  return Content{std::move(out)};
                },
                [](const auto &number) -> auto { return Content{number}; },
            },
            one.Value),
    };
  }

  auto Appended(std::vector<std::byte> &out, const Tags::Tag &one) -> bool
  {
    if (const auto *bytes = std::get_if<Tags::Chunk>(&one.Value))
    {
      Append(out, bytes->Bytes);
      return true;
    }
    return Unhexed(out, Tags::Text(one));
  }

  auto Put(Writing &to, const Tag &one) -> std::expected<void, Failure>
  {
    return std::visit(
        DxfIO::Match{
            [&to, &one](const std::string &text) -> std::expected<void, Failure>
            {
              return Verbatim(one.Code) ? Word(to, one.Code, text)
                                        : Value<std::string>::Write(to, one.Code, text);
            },
            [&to, &one](Handle named) -> std::expected<void, Failure>
            {
              const std::string written = HexOf(named);
              return to.To->Put(Tags::Tag{
                  .Code  = one.Code,
                  .Value = std::string_view{written},
              });
            },
            [&to, &one](const Chunk &bytes) -> std::expected<void, Failure>
            {
              return to.To->Put(
                  Tags::Tag{.Code = one.Code, .Value = Tags::Chunk{Viewed(bytes.Bytes)}});
            },
            [&to, &one](const auto &number) -> auto
            { return to.To->Put(Tags::Tag{.Code = one.Code, .Value = number}); },
        },
        one.Value);
  }

  auto Put(Writing &to, std::span<const Tag> all) -> std::expected<void, Failure>
  {
    return Chained(all,
                   [&](const Tag &one) -> std::expected<void, Failure> { return Put(to, one); });
  }
} // namespace DxfIO::Fields
