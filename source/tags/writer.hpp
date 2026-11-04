#ifndef DXFIO_TAGS_WRITER_HPP
#define DXFIO_TAGS_WRITER_HPP

#include "tags/tag.hpp"

#include "dxfio/api.hpp"
#include "dxfio/failure.hpp"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

namespace DxfIO::Tags
{
enum class Form : std::uint8_t
{
  Ascii,
  Wide,
  Narrow,
};

class DXFIO_API Writer
{
public:
  explicit Writer(std::string &out, Form form = Form::Ascii);

  auto Put(const Tag &one) -> std::expected<void, Failure>;

private:
  void Spell(int code, const Content &value);
  auto Pack(int code, const Content &value) -> std::expected<void, Failure>;
  void Pair(int code, std::string_view text);
  void Coded(int code);

  std::string *m_Out;
  std::size_t m_Line{};
  Form m_Form{Form::Ascii};
};
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_WRITER_HPP */
