#include "dxfio/create.hpp"

#include "down/down.hpp"
#include "empty/whole.hpp"
#include "handseed.hpp"
#include "header/created.hpp"
#include "past.hpp"
#include "seed.hpp"

namespace DxfIO
{
  auto Create(Version release) -> Document
  {
    Seed handles;
    if (release >= Version::R13)
    {
      return Empty::Whole(release, handles);
    }
    Document out = Down::Narrowed(Empty::Whole(Version::R2000, handles), release, nullptr);
    out.Header   = Header::Created(release);
    WriteSeed(out, Past(out));
    return out;
  }
} // namespace DxfIO
