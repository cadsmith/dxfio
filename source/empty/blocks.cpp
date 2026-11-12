#include "blocks.hpp"

#include "empty/space.hpp"

#include <string>
#include <utility>

namespace DxfIO::Empty
{
  namespace
  {
    auto Owned(Space space, Seed &handles) -> Entities::Record
    {
      Entities::Record out{};
      out.Base.Self  = handles.Take();
      out.Base.Owner = space.Record;
      out.Paper      = space.Paper;
      return out;
    }
  } // namespace

  auto Block(std::string name, Space space, Seed &handles) -> Blocks::Block
  {
    Blocks::Block out{};
    out.Begin.Record = Owned(space, handles);
    out.Begin.Name   = std::move(name);
    out.End.Record   = Owned(space, handles);
    return out;
  }

  auto Blocks(const Roots &ids, Seed &handles) -> std::vector<Blocks::Block>
  {
    std::vector<Blocks::Block> out;
    out.push_back(Block("*Model_Space", Space{.Record = ids.Records.Model}, handles));
    out.push_back(
        Block("*Paper_Space", Space{.Record = ids.Records.Paper, .Paper = true}, handles));
    return out;
  }
} // namespace DxfIO::Empty
