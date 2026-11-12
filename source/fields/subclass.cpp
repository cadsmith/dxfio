#include "fields/subclass.hpp"

#include "tags/codes.hpp"

namespace DxfIO::Fields
{
  void Open(const Subclass &shape, Walk &in)
  {
    static_cast<void>(in.Subclass(shape.Marker));
  }

  auto Mark(const Subclass &shape, const Writing &to) -> std::expected<void, Failure>
  {
    if (to.Release < Version::R13)
    {
      return {};
    }
    return to.To->Put(Tags::Tag{.Code = Tags::Codes::Marker, .Value = shape.Marker});
  }
} // namespace DxfIO::Fields
