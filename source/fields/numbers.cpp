#include "fields/numbers.hpp"

#include <utility>
#include <variant>

namespace DxfIO::Fields
{
  namespace
  {
    auto Integer(const Tags::Tag *one) -> std::optional<std::int64_t>
    {
      if (one == nullptr)
      {
        return std::nullopt;
      }
      if (const auto *held = std::get_if<std::int64_t>(&one->Value))
      {
        return *held;
      }
      if (const auto *flag = std::get_if<bool>(&one->Value))
      {
        return *flag ? 1 : 0;
      }
      return std::nullopt;
    }

    auto Small(const Tags::Tag &one) -> bool
    {
      const std::optional<std::int64_t> held = Integer(&one);
      return held && std::in_range<int>(*held);
    }
  } // namespace

  auto Value<int>::Read(Walk &in, int code) -> std::optional<int>
  {
    return Integer(in.Take(code, Small))
        .transform([](std::int64_t held) -> int { return static_cast<int>(held); });
  }

  auto Value<int>::Write(Writing &to, int code, int held) -> std::expected<void, Failure>
  {
    return to.To->Put(Tags::Tag{.Code = code, .Value = std::int64_t{held}});
  }

  auto Value<std::int64_t>::Read(Walk &in, int code) -> std::optional<std::int64_t>
  {
    return Integer(in.Take(code));
  }

  auto Value<std::int64_t>::Write(Writing &to, int code, std::int64_t held)
      -> std::expected<void, Failure>
  {
    return to.To->Put(Tags::Tag{.Code = code, .Value = held});
  }

  auto Value<double>::Read(Walk &in, int code) -> std::optional<double>
  {
    const Tags::Tag *one  = in.Take(code);
    const auto      *held = one == nullptr ? nullptr : std::get_if<double>(&one->Value);
    return held == nullptr ? std::nullopt : std::optional{*held};
  }

  auto Value<double>::Write(Writing &to, int code, double held) -> std::expected<void, Failure>
  {
    return to.To->Put(Tags::Tag{.Code = code, .Value = held});
  }

  auto Value<bool>::Read(Walk &in, int code) -> std::optional<bool>
  {
    return Integer(in.Take(code)).transform([](std::int64_t held) -> bool { return held != 0; });
  }

  auto Value<bool>::Write(Writing &to, int code, bool held) -> std::expected<void, Failure>
  {
    return to.To->Put(Tags::Tag{.Code = code, .Value = held});
  }
} // namespace DxfIO::Fields
