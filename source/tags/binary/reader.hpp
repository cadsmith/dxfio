#ifndef DXFIO_TAGS_BINARY_READER_HPP
#define DXFIO_TAGS_BINARY_READER_HPP

#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <expected>
#include <optional>
#include <string_view>
#include <vector>

namespace DxfIO::Tags::Binary
{
class DXFIO_API Reader
{
public:
  // Reads the bytes after the sentinel as tags; a group whose value is no value of its type is left out and told
  // to issues, when given.
  Reader(std::string_view bytes, std::vector<Issue> *issues);

  [[nodiscard]] auto Done() const -> bool;

  // The next tag, none for a group left out, or the failure that stops the file.
  auto Next() -> std::expected<std::optional<Tag>, Failure>;

private:
  auto Code() -> std::optional<int>;
  void Tell(Issue issue);

  std::string_view m_Rest;
  std::vector<Issue> *m_Issues;
  bool m_Wide{};
};
} // namespace DxfIO::Tags::Binary

#endif /* DXFIO_TAGS_BINARY_READER_HPP */
