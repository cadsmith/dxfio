#ifndef DXFIO_OBJECTS_SHAPES_DATA_HPP
#define DXFIO_OBJECTS_SHAPES_DATA_HPP

#include "chained.hpp"
#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/numbers.hpp"
#include "fields/owned.hpp"
#include "fields/sequence.hpp"
#include "fields/tally.hpp"
#include "fields/text.hpp"
#include "shapes.hpp"

#include "dxfio/objects/datatable.hpp"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace DxfIO
{
namespace Objects
{
inline constexpr int Typed = 92;

inline constexpr int Titled = 2;

inline auto Columned(std::vector<DxfIO::Group> all) -> std::optional<std::vector<Column>>
{
  std::vector<Column> out;
  for (std::size_t at = 0; at < all.size(); ++at)
  {
    DxfIO::Group &one = all.at(at);
    const bool opens = one.Code == Typed && at + 1 < all.size() && all.at(at + 1).Code == Titled;
    if (opens)
    {
      const std::int64_t kind = std::get<std::int64_t>(one.Value);
      if (!std::in_range<int>(kind))
      {
        return std::nullopt;
      }
      out.push_back(Column{
          .Kind = static_cast<int>(kind),
          .Name = std::move(std::get<std::string>(all.at(at + 1).Value)),
          .Cells = {},
      });
      ++at;
    }
    else if (out.empty())
    {
      return std::nullopt;
    }
    else
    {
      out.back().Cells.push_back(std::move(one));
    }
  }
  return out;
}

template <Edition KindV>
void Filled(DataTableOf<KindV> &one, Fields::Walk &in)
{
  if (std::optional<std::vector<Column>> read = Columned(in.Rest()))
  {
    one.Columns = std::move(*read);
    in.Spend();
  }
}

template <Edition KindV>
auto Filling(const DataTableOf<KindV> &one, Fields::Writing &to) -> std::expected<void, Failure>
{
  return Chained(one.Columns,
                 [&to](const Column &column) -> std::expected<void, Failure>
                 {
                   return Fields::Value<int>::Write(to, Typed, column.Kind)
                       .and_then([&] -> auto { return Fields::Value<std::string>::Write(to, Titled, column.Name); })
                       .and_then([&] -> auto { return Fields::Put(to, column.Cells); });
                 });
}
} // namespace Objects

template <Objects::Edition KindV>
inline constexpr std::string_view Marked = KindV == Objects::Edition::Early ? "ACDBDATATABLE" : "AcDbDataTable";

template <Objects::Edition KindV>
inline constexpr auto Shapes<Objects::DataTableOf<KindV>> = std::tuple{
    Fields::Sequence{.Marker = Marked<KindV>},
    Fields::Field{.Code = 70, .Member = &Objects::DataTableOf<KindV>::Version},
    Fields::Tally{.Code = 90, .Member = &Objects::DataTableOf<KindV>::Columns},
    Fields::Field{.Code = 91, .Member = &Objects::DataTableOf<KindV>::Rows},
    Fields::Field{.Code = 1, .Member = &Objects::DataTableOf<KindV>::Name},
    Fields::Custom<Objects::DataTableOf<KindV>>{
        .Read = Objects::Filled<KindV>,
        .Write = Objects::Filling<KindV>,
    },
};

template <>
inline constexpr std::string_view Type<Objects::DataTable> = "DATATABLE";

template <>
inline constexpr std::string_view Type<Objects::AcDbDataTable> = "ACDBDATATABLE";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_DATA_HPP */
