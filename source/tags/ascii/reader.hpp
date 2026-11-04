#ifndef DXFIO_TAGS_ASCII_READER_HPP
#define DXFIO_TAGS_ASCII_READER_HPP

#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"
#include "dxfio/issue.hpp"

#include <cstddef>
#include <expected>
#include <optional>
#include <string_view>
#include <vector>

namespace DxfIO::Tags::Ascii
{
class DXFIO_API Reader
{
public:
  // Reads the text as tags; a group whose value does not parse is left out and told to issues, when given.
  Reader(std::string_view text, std::vector<Issue> *issues);

  [[nodiscard]] auto Done() const -> bool;

  // The next tag, none for a group left out, or the failure that stops the file.
  auto Next() -> std::expected<std::optional<Tag>, Failure>;

private:
  auto Line() -> std::string_view;
  void Tell(Issue issue);

  std::string_view m_Text;
  std::vector<Issue> *m_Issues;
  std::size_t m_At{};
  std::size_t m_Line{};
};
} // namespace DxfIO::Tags::Ascii

#endif /* DXFIO_TAGS_ASCII_READER_HPP */
