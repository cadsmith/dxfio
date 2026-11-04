#include "records.hpp"

#include "file/cursor.hpp"

namespace DxfIO::File::Sections
{
auto Records(std::span<const Tags::Tag> body) -> std::expected<std::vector<Record>, Failure>
{
  std::vector<Record> out;
  Cursor in{body};
  while (!in.Done())
  {
    std::expected<Record, Failure> one = in.Take();
    if (!one)
    {
      return std::unexpected{one.error()};
    }
    out.push_back(*one);
  }
  return out;
}
} // namespace DxfIO::File::Sections
