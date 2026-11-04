#include "dxfio/create.hpp"

#include "down/down.hpp"
#include "empty/blocks.hpp"
#include "empty/classes.hpp"
#include "empty/objects.hpp"
#include "empty/roots.hpp"
#include "empty/tables.hpp"
#include "handseed.hpp"
#include "header/created.hpp"
#include "past.hpp"
#include "seed.hpp"

#include <algorithm>
#include <string_view>
#include <vector>

namespace DxfIO
{
namespace
{
void Pointed(std::vector<Header::Variable> &all, std::string_view name, Handle to)
{
  const auto found = std::ranges::find(all, name, &Header::Variable::Name);
  if (found != all.end())
  {
    found->Values.front().Value = to;
  }
}

void Pruned(Document &drawing)
{
  drawing.Dimstyle.Head.Base.Extra.clear();
  for (Tables::BlockRecord &one : drawing.BlockRecord.Records)
  {
    one.Layout = {};
  }
  Tables::Layer &zero = drawing.Layer.Records.front();
  zero.Style = {};
  zero.Material = {};
}

auto Whole(Version release) -> Document
{
  Seed handles;
  const Empty::Roots ids = Empty::Taken(handles);
  Document out{};
  out.Order = {"HEADER", "CLASSES", "TABLES", "BLOCKS", "ENTITIES", "OBJECTS"};
  Empty::Tables(out, ids, handles);
  out.Blocks = Empty::Blocks(ids, handles);
  const Handle active = out.Vport.Records.front().Record.Base.Self;
  out.Objects = release >= Version::R2000 ? Empty::Objects(ids, active, handles) : Empty::Early(ids, handles);
  out.Classes = Empty::Classes(release, out.Objects);
  if (release < Version::R2000)
  {
    Pruned(out);
  }
  out.Header = Header::Created(release);
  Pointed(out.Header, "$CMATERIAL", ids.Materials.Bylayer);
  Pointed(out.Header, "$INTERFEREOBJVS", Handle{});
  Pointed(out.Header, "$INTERFEREVPVS", Handle{});
  WriteSeed(out, handles.Next());
  return out;
}
} // namespace

auto Create(Version release) -> Document
{
  if (release >= Version::R13)
  {
    return Whole(release);
  }
  Document out = Down::Narrowed(Whole(Version::R2000), release, nullptr);
  out.Header = Header::Created(release);
  WriteSeed(out, Past(out));
  return out;
}
} // namespace DxfIO
