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

    /**
     *  Takes bytes another writer of the same form wrote, as that many lines.
     */
    void Append(std::string_view bytes, std::size_t lines);

    /**
     *  The form written, which a writer taking its text must share.
     */
    [[nodiscard]] auto Formed() const -> Tags::Form { return m_Form; }

    /**
     *  The lines written so far: two a tag in ASCII, one in binary.
     */
    [[nodiscard]] auto Lines() const -> std::size_t { return m_Line; }

  private:
    void Spell(int code, const Content &value);
    auto Pack(int code, const Content &value) -> std::expected<void, Failure>;
    void Padded(int code, std::int64_t number);
    void Pair(int code, std::string_view text);
    void Coded(int code);

    std::string *m_Out;
    std::size_t  m_Line{};
    Form         m_Form{Form::Ascii};
  };
} // namespace DxfIO::Tags

#endif /* DXFIO_TAGS_WRITER_HPP */
