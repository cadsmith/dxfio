#include "oracle.hpp"

#include "header/io.hpp"

#include "dxfio/load.hpp"
#include "dxfio/save.hpp"

namespace DxfIO
{
  auto Broken(std::string_view bytes) -> std::string
  {
    const auto first = Load(bytes);
    if (!first)
    {
      return {};
    }
    const Options with{.Release = Header::Declared(first->Document.Header)};
    const auto    once = Save(first->Document, with);
    if (!once)
    {
      return "the document read did not save: " + once.error().What;
    }
    const auto second = Load(once->Bytes);
    if (!second)
    {
      return "the text saved did not load: " + second.error().What;
    }
    const auto twice = Save(second->Document, with);
    if (!twice)
    {
      return "the document read again did not save: " + twice.error().What;
    }
    if (twice->Bytes != once->Bytes)
    {
      return "saved again, the text changed";
    }
    for (const Version release : {Version::R10, Version::R12, Version::R13, Version::R14,
                                  Version::R2000, Version::R2007, Version::R2018})
    {
      const auto other = Save(first->Document, Options{.Release = release});
      if (!other)
      {
        continue;
      }
      if (!Load(other->Bytes))
      {
        return "saved to another release, the text did not load";
      }
    }
    return {};
  }
} // namespace DxfIO
