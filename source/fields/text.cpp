#include "fields/text.hpp"

#include <string_view>

namespace DxfIO::Fields
{
  auto Value<std::string>::Read(Walk &in, int code) -> std::optional<std::string>
  {
    const Tags::Tag *one = in.Take(code);
    if (one == nullptr)
    {
      return std::nullopt;
    }
    return Encoding::Decode(in.With().Text, Tags::Text(*one));
  }

  auto Value<std::string>::Write(Writing &to, int code, std::string_view held)
      -> std::expected<void, Failure>
  {
    const std::string written = Encoding::Encode(to.Text, held);
    return to.To->Put(Tags::Tag{.Code = code, .Value = std::string_view{written}});
  }

  auto Word(Writing &to, int code, std::string_view text) -> std::expected<void, Failure>
  {
    if (text.find_first_of("\r\n") == std::string_view::npos)
    {
      return to.To->Put(Tags::Tag{.Code = code, .Value = text});
    }
    return Value<std::string>::Write(to, code, text);
  }

  auto Value<Symbol>::Read(Walk &in, int code) -> std::optional<Symbol>
  {
    return Value<std::string>::Read(in, code).transform([](const std::string &text) -> Symbol
                                                        { return Symbol{text}; });
  }

  auto Value<Symbol>::Write(Writing &to, int code, const Symbol &held)
      -> std::expected<void, Failure>
  {
    return Value<std::string>::Write(to, code, held.Str());
  }
} // namespace DxfIO::Fields
