#include "../filing.hpp"
#include "blocks/io.hpp"
#include "chained.hpp"
#include "entities/io.hpp"
#include "shapes/block.hpp"
#include "shapes/endblk.hpp"

#include "dxfio/blocks/definition.hpp"

#include <utility>

namespace DxfIO::Blocks
{
  namespace
  {
    /**
     *  The ENDBLK a file left out: on the block's layer, owned by its block record, with no handle
     *  until an audit gives it one.
     */
    auto Closed(const Block &block) -> EndBlk
    {
      EndBlk out{};
      out.Record.Layer      = block.Record.Layer;
      out.Record.Base.Owner = block.Record.Base.Owner;
      out.Record.Paper      = block.Record.Paper;
      return out;
    }
  } // namespace

  auto Read(std::span<const File::Nest> all, const Fields::Reading &with) -> std::vector<Definition>
  {
    std::vector<Definition> out;
    out.reserve(all.size());
    for (const File::Nest &one : all)
    {
      auto   block    = Taken<Block>(one.Head, with);
      auto   entities = Entities::Read(one.Records, with);
      EndBlk endblk   = one.End ? Taken<EndBlk>(*one.End, with) : Closed(block);
      out.push_back(Definition{
          .Block    = std::move(block),
          .Entities = std::move(entities),
          .EndBlk   = std::move(endblk),
      });
    }
    return out;
  }

  auto Write(Fields::Writing &to, std::span<const Definition> all) -> std::expected<void, Failure>
  {
    return Chained(all,
                   [&](const Definition &one) -> auto
                   {
                     return Given(to, one.Block)
                         .and_then([&to, &one] -> std::expected<void, Failure>
                                   { return Entities::Write(to, one.Entities); })
                         .and_then([&to, &one] -> std::expected<void, Failure>
                                   { return Given(to, one.EndBlk); });
                   });
  }
} // namespace DxfIO::Blocks
