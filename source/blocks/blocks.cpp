#include "../filing.hpp"
#include "blocks/io.hpp"
#include "chained.hpp"
#include "entities/io.hpp"
#include "shapes/block.hpp"
#include "shapes/endblk.hpp"

#include "dxfio/blocks/definition.hpp"

namespace DxfIO::Blocks
{
  auto Read(std::span<const File::Nest> all, const Fields::Reading &with) -> std::vector<Definition>
  {
    std::vector<Definition> out;
    out.reserve(all.size());
    for (const File::Nest &one : all)
    {
      out.push_back(Definition{
          .Begin    = Taken<Block>(one.Head, with),
          .Entities = Entities::Read(one.Records, with),
          .End      = Taken<EndBlk>(one.End, with),
      });
    }
    return out;
  }

  auto Write(Fields::Writing &to, std::span<const Definition> all) -> std::expected<void, Failure>
  {
    return Chained(all,
                   [&](const Definition &one) -> auto
                   {
                     return Given(to, one.Begin)
                         .and_then([&to, &one] -> std::expected<void, Failure>
                                   { return Entities::Write(to, one.Entities); })
                         .and_then([&to, &one] -> std::expected<void, Failure>
                                   { return Given(to, one.End); });
                   });
  }
} // namespace DxfIO::Blocks
