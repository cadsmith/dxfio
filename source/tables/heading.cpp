#include "heading.hpp"

#include "../filing.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/subclass.hpp"

#include <string>
#include <tuple>

namespace DxfIO::Tables
{
namespace
{
constexpr auto Named = std::tuple{
    Fields::Field{.Code = 2, .Member = &Head::Name},
};

constexpr auto Counted = std::tuple{
    Fields::Subclass{.Marker = "AcDbSymbolTable"},
    Fields::Field{.Code = 70, .Member = &Head::Count},
};
} // namespace
} // namespace DxfIO::Tables

namespace DxfIO
{
template <>
constexpr auto Shapes<Tables::Head> = std::tuple_cat(Tables::Named, Tables::Counted);
} // namespace DxfIO

namespace DxfIO::Tables
{
auto Headed(const File::Record &one, const Fields::Reading &with) -> Tables::Head
{
  return Taken<Tables::Head>(one, with);
}

auto Headed(Fields::Writing &to, const Tables::Head &one) -> std::expected<void, Failure>
{
  const auto extra = [&to, &one] -> std::expected<void, Failure>
  {
    return to.Release >= Version::R13 ? Fields::Put(to, one.Base.Extra) : std::expected<void, Failure>{};
  };
  return Fields::Word(to, 0, "TABLE")
      .and_then([&to, &one] -> std::expected<void, Failure> { return Fields::Write(one, to, Named); })
      .and_then([&to, &one] -> std::expected<void, Failure> { return DxfIO::Head(to, one.Base, Handled); })
      .and_then([&to, &one] -> std::expected<void, Failure> { return Fields::Write(one, to, Counted); })
      .and_then(extra)
      .and_then([&to, &one] -> std::expected<void, Failure> { return Tail(to, one.Base); });
}
} // namespace DxfIO::Tables
