#ifndef DXFIO_ENTITIES_SHAPES_SEQUENCE_HPP
#define DXFIO_ENTITIES_SHAPES_SEQUENCE_HPP

#include "../../filing.hpp"
#include "chained.hpp"
#include "file/record.hpp"
#include "seqend.hpp"

#include "dxfio/entities/sequence.hpp"

#include <algorithm>
#include <cstddef>
#include <expected>
#include <iterator>
#include <span>
#include <string_view>
#include <vector>

namespace DxfIO::Entities
{
template <class ChildT>
void Gather(Sequence<ChildT> &into, std::span<const File::Record> &rest, const Fields::Reading &with)
{
  const auto run = std::ranges::find_if(rest, [](const File::Record &one) -> bool { return one.Name != Type<ChildT>; });
  const auto count = static_cast<std::size_t>(std::ranges::distance(rest.begin(), run));
  into.Children.reserve(count);
  for (const File::Record &one : rest.first(count))
  {
    into.Children.push_back(Taken<ChildT>(one, with));
  }
  rest = rest.subspan(count);
  if (!rest.empty() && rest.front().Name == Type<Seqend>)
  {
    into.Closing = Boxed{Taken<Seqend>(rest.front(), with)};
    rest = rest.subspan(1);
  }
}

template <class ChildT>
auto Give(Fields::Writing &to, const Sequence<ChildT> &from) -> std::expected<void, Failure>
{
  const auto closing = [&to, &from] -> auto
  {
    return from.Closing ? Given(to, **from.Closing) : std::expected<void, Failure>{};
  };
  return Chained(from.Children, [&to](const ChildT &one) -> auto { return Given(to, one); }).and_then(closing);
}
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SHAPES_SEQUENCE_HPP */
