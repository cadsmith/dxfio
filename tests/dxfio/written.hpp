#ifndef DXFIO_TESTS_WRITTEN_HPP
#define DXFIO_TESTS_WRITTEN_HPP

#include "dxfio/save.hpp"

#include <expected>
#include <string>
#include <utility>
#include <vector>

namespace DxfIO::Tests
{
// Saves a drawing to its bytes, handing what the writer noted to notes when asked.
inline auto Written(const Document &drawing, const Options &with = {}, std::vector<Issue> *notes = nullptr)
    -> std::expected<std::string, Failure>
{
  return Save(drawing, with)
      .transform(
          [notes](Saved saved) -> std::string
          {
            if (notes != nullptr)
            {
              *notes = std::move(saved.Issues);
            }
            return std::move(saved.Bytes);
          });
}
} // namespace DxfIO::Tests

#endif /* DXFIO_TESTS_WRITTEN_HPP */
