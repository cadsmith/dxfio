#include "fields/picture.hpp"

#include "bytes.hpp"
#include "fields/numbers.hpp"
#include "fields/owned.hpp"
#include "tags/tag.hpp"

#include "dxfio/version.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace DxfIO::Fields
{
  namespace
  {
    constexpr int Older = 92;

    constexpr int Larger = 160;

    constexpr int Bytes = 310;

    constexpr std::array<int, 1> Pieces = {Bytes};
  } // namespace

  auto Pictured(Walk &in) -> std::vector<std::byte>
  {
    static_cast<void>(Value<std::int64_t>::Read(in, Older));
    static_cast<void>(Value<std::int64_t>::Read(in, Larger));
    std::vector<std::byte> out;
    for (const Tags::Tag *one : in.Gather(Pieces))
    {
      if (!Appended(out, *one))
      {
        in.Refuse();
      }
    }
    return out;
  }

  auto Show(Writing &to, const std::vector<std::byte> &held) -> std::expected<void, Failure>
  {
    if (held.empty() || to.Release != to.From)
    {
      return {};
    }
    const int  code = to.Release < Version::R2010 ? Older : Larger;
    const auto size = static_cast<std::int64_t>(held.size());
    return Value<std::int64_t>::Write(to, code, size)
        .and_then(
            [&] -> std::expected<void, Failure>
            {
              return to.To->Put(Tags::Tag{
                  .Code  = Bytes,
                  .Value = Tags::Chunk{Viewed(held)},
              });
            });
  }
} // namespace DxfIO::Fields
