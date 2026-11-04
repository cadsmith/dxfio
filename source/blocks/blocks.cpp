#include "dxfio/blocks/blocks.hpp"

#include "../filing.hpp"
#include "blocks/io.hpp"
#include "chained.hpp"
#include "entities/io.hpp"
#include "shapes/begin.hpp"
#include "shapes/end.hpp"

namespace DxfIO::Blocks
{
auto Read(std::span<const File::Nest> all, const Fields::Reading &with) -> std::vector<Block>
{
  std::vector<Block> out;
  out.reserve(all.size());
  for (const File::Nest &one : all)
  {
    out.push_back(Block{
        .Begin = Taken<Begin>(one.Head, with),
        .Entities = Entities::Read(one.Records, with),
        .End = Taken<End>(one.End, with),
    });
  }
  return out;
}

auto Write(Fields::Writing &to, std::span<const Block> all) -> std::expected<void, Failure>
{
  return Chained(all,
                 [&](const Block &one) -> auto
                 {
                   return Given(to, one.Begin)
                       .and_then([&to, &one] -> std::expected<void, Failure>
                                 { return Entities::Write(to, one.Entities); })
                       .and_then([&to, &one] -> std::expected<void, Failure> { return Given(to, one.End); });
                 });
}
} // namespace DxfIO::Blocks
