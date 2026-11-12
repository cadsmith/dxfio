#include "fields/handles.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Fields
{
  namespace
  {
    auto Hexed(const Tags::Tag &one) -> bool
    {
      return Parsed(Tags::Text(one)).has_value();
    }
  } // namespace

  auto Value<Handle>::Read(Walk &in, int code) -> std::optional<Handle>
  {
    const Tags::Tag *one = in.Take(code, Hexed);
    return one == nullptr ? std::nullopt : Parsed(Tags::Text(*one));
  }

  auto Value<Handle>::Write(Writing &to, int code, Handle held) -> std::expected<void, Failure>
  {
    const std::string written = Hex(held);
    return to.To->Put(Tags::Tag{.Code = code, .Value = std::string_view{written}});
  }
} // namespace DxfIO::Fields
