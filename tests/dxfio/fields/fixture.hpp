#ifndef DXFIO_TESTS_DXF_FIELDS_FIXTURE_HPP
#define DXFIO_TESTS_DXF_FIELDS_FIXTURE_HPP

#include "fields/context.hpp"
#include "tags/writer.hpp"

#include <string>

namespace DxfIO::Fields
{
struct Sheet
{
  explicit Sheet(Version release = Version::R2018, Encoding::Scheme text = Encoding::Utf8())
      : To{.To = &Writer, .Release = release, .Text = text, .From = release}
  {
  }

  Sheet(const Sheet &) = delete;
  Sheet(Sheet &&) = delete;
  auto operator=(const Sheet &) -> auto;
  auto operator=(Sheet &&) -> auto;
  ~Sheet() = default;

  std::string Out;
  Tags::Writer Writer{Out};
  Writing To;
};
} // namespace DxfIO::Fields

#endif /* DXFIO_TESTS_DXF_FIELDS_FIXTURE_HPP */
