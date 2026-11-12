#include "nests.hpp"

#include "file/cursor.hpp"

#include <string_view>
#include <utility>

namespace DxfIO::File::Sections
{
  namespace
  {
    auto Taken(Cursor &in, std::string_view open, std::string_view close)
        -> std::expected<Nest, Failure>
    {
      std::expected<Record, Failure> head = in.Take(open);
      if (!head)
      {
        return std::unexpected{head.error()};
      }
      std::expected<std::vector<Record>, Failure> inside = in.Until(close);
      if (!inside)
      {
        return std::unexpected{inside.error()};
      }
      return in.Take(close).transform(
          [&head, &inside](const Record &end) -> Nest
          { return Nest{.Head = *head, .Records = std::move(*inside), .End = end}; });
    }

    auto All(std::span<const Tags::Tag> body, std::string_view open, std::string_view close)
        -> std::expected<std::vector<Nest>, Failure>
    {
      std::vector<Nest> out;
      Cursor            in{body};
      while (!in.Done())
      {
        std::expected<Nest, Failure> one = Taken(in, open, close);
        if (!one)
        {
          return std::unexpected{one.error()};
        }
        out.push_back(std::move(*one));
      }
      return out;
    }
  } // namespace

  auto Tables(std::span<const Tags::Tag> body) -> std::expected<std::vector<Nest>, Failure>
  {
    return All(body, "TABLE", "ENDTAB");
  }

  auto Blocks(std::span<const Tags::Tag> body) -> std::expected<std::vector<Nest>, Failure>
  {
    return All(body, "BLOCK", "ENDBLK");
  }
} // namespace DxfIO::File::Sections
