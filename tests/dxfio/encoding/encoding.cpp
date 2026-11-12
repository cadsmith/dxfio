#include "encoding/encoding.hpp"

#include <doctest/doctest.h>

#include <array>
#include <string>
#include <string_view>

namespace DxfIO::Encoding
{
  namespace
  {
    constexpr std::string_view Mixed =
        "Plan \xd8\xa7\xd9\x84\xd8\xb3\xd9\x84\xd8\xa7\xd9\x85 n\xc2\xb0 3 "
        "\xe2\x82\xac \xf0\x9f\x98\x80";
  } // namespace

  TEST_CASE("encoding: a code page's bytes and escapes read as UTF-8")
  {
    const Scheme latin  = Paged(Standard());
    const Scheme arabic = Paged(*Find("ANSI_1256"));
    CHECK(Decode(latin, "caf\xe9") == "caf\xc3\xa9");
    CHECK(Decode(latin, "\x80")    == "\xe2\x82\xac");
    CHECK(Decode(arabic, "\xc7\xe1\xd3\xe1\xc7\xe3")
          == "\xd8\xa7\xd9\x84\xd8\xb3\xd9\x84\xd8\xa7\xd9\x85");
    CHECK(Decode(latin, "a\\U+0627z")       == "a\xd8\xa7z");
    CHECK(Decode(latin, "\\U+D83D\\U+DE00") == "\xf0\x9f\x98\x80");
    CHECK(Decode(latin, "a^Jb^ c")          == "a\nb^c");
  }

  TEST_CASE("encoding: a file of R2007 or later keeps UTF-8, escapes read")
  {
    CHECK(Decode(Utf8(), "x\\U+0627\xd8\xa8") == "x\xd8\xa7\xd8\xa8");
    CHECK(Encode(Utf8(), "\xd8\xa7\n")        == "\xd8\xa7^J");
  }

  TEST_CASE("encoding: a damaged byte of a UTF-8 file read as U+FFFD")
  {
    CHECK(Decode(Utf8(), "a\xff\xe7"
                         "b")
          == "a\xef\xbf\xbd\xef\xbf\xbd"
             "b");
    CHECK(Decode(Utf8(), "\xff\xe7") == Decode(Utf8(), "\xff\xff"));
  }

  TEST_CASE("encoding: what comes before a damaged byte of a UTF-8 file kept as it is")
  {
    CHECK(Decode(Utf8(), "\xd8\xa7\xe2\x82\xac\xff\xd8\xa7")
          == "\xd8\xa7\xe2\x82\xac\xef\xbf\xbd\xd8\xa7");
    CHECK(Decode(Utf8(), "\xef\xbf\xbd\xe2\x82")     == "\xef\xbf\xbd\xef\xbf\xbd\xef\xbf\xbd");
    CHECK(Decode(Utf8(), "\xd8\xa7\xf0\x9f\x98\x80") == "\xd8\xa7\xf0\x9f\x98\x80");
  }

  TEST_CASE("encoding: a point the page lacks is written as an escape")
  {
    const Scheme latin = Paged(Standard());
    CHECK(Encode(latin, "caf\xc3\xa9")      == "caf\xe9");
    CHECK(Encode(latin, "\xd8\xa7")         == "\\U+0627");
    CHECK(Encode(latin, "\xf0\x9f\x98\x80") == "\\U+D83D\\U+DE00");
    CHECK(Encode(Paged(*Find("ANSI_1256")), "\xd8\xa7\xd9\x84\xd8\xb3") == "\xc7\xe1\xd3");
  }

  TEST_CASE("encoding: a backslash that would start an escape is one")
  {
    CHECK(Encode(Utf8(), "\\U+0041")             == "\\U+005CU+0041");
    CHECK(Encode(Paged(Standard()), "a\\U+0041") == "a\\U+005CU+0041");
    CHECK(Encode(Utf8(), "\\U+D83D")             == "\\U+D83D");
    CHECK(Encode(Utf8(), "C:\\Users")            == "C:\\Users");
    CHECK(Decode(Utf8(), "\\U+005CU+0041")       == "\\U+0041");
  }

  TEST_CASE("encoding: text reads back as written, on one line")
  {
    const std::array schemes{Utf8(), Paged(Standard()), Paged(*Find("ANSI_1256")),
                             Paged(*Find("ANSI_874"))};
    for (const Scheme &with : schemes)
    {
      for (const std::string_view text :
           {Mixed, std::string_view{"one\ntwo^\x01 \\U+0041 \\U+D83D"}})
      {
        const std::string written = Encode(with, text);
        CHECK(written.find_first_of("\r\n") == std::string::npos);
        CHECK(Decode(with, written)         == text);
      }
    }
  }

  TEST_CASE("encoding: a release's scheme, UTF-8 from R2007")
  {
    const Codepage &arabic = *Find("ANSI_1256");
    CHECK(Of(Version::R2007, arabic).Page == nullptr);
    CHECK(Of(Version::R2018, arabic).Page == nullptr);
    CHECK(Of(Version::R2004, arabic).Page == &arabic);
    CHECK(Of(Version::R12, arabic).Page   == &arabic);
  }

  TEST_CASE("encoding: an escape of what the page holds comes back as its byte")
  {
    const Scheme latin = Paged(Standard());
    CHECK(Encode(latin, Decode(latin, "mosqu\\U+00E9e"))
          == "mosqu\xe9"
             "e");
  }

  TEST_CASE("encoding: two bytes a character, a trail byte no escape")
  {
    const Scheme      japanese = Paged(*Find("ANSI_932"));
    const std::string raw      = "\x82\xa0\x81\x5cU+0041 \xb1";
    const std::string text     = Decode(japanese, raw);
    CHECK(text == "\u3042\u2015U+0041 \uff71");
    CHECK(Encode(japanese, text)           == raw);
    CHECK(Decode(japanese, "\x82")         == "\u0082");
    CHECK(Encode(japanese, "\u0082\u3042") == "\\U+0082\x82\xa0");
    CHECK(Encode(japanese, "\u0080")       == "\x80");
    CHECK(Encode(japanese, "\uff71")       == "\xb1");
    CHECK(Encode(Paged(*Find("ANSI_950")), "\u0081") == "\x81");
    CHECK(Decode(japanese, "\x82\x20")  == "\u0082 ");
    CHECK(Encode(japanese, "\u0627")    == "\\U+0627");
    CHECK(Decode(japanese, "\\M+182A0") == "\u3042");
    CHECK(Encode(japanese, "\\M+182A0") == "\\U+005CM+182A0");
  }

  TEST_CASE("encoding: opaque, every byte back as it came")
  {
    std::string all;
    for (int byte = 0x20; byte < 0x100; ++byte)
    {
      all.push_back(static_cast<char>(byte));
    }
    CHECK(Encode(Opaque(), Decode(Opaque(), all)) == all);
  }

  TEST_CASE("encoding: opaque leaves carets and escapes as they stand")
  {
    for (const std::string_view raw : {"a^Jb", "a^ b", "\x81^\xd6", "\x81^1", "\\U+0041"})
    {
      CAPTURE(raw);
      CHECK(Encode(Opaque(), Decode(Opaque(), raw)) == raw);
    }
    CHECK(Decode(Opaque(), "^J") == "^J");
  }

  TEST_CASE("encoding: opaque keeps a control character off the line")
  {
    CHECK(Encode(Opaque(), "a\nb") == "a^Jb");
  }
} // namespace DxfIO::Encoding

namespace DxfIO::Encoding
{
  TEST_CASE("encoding: the last byte of a double byte page leads nothing")
  {
    CHECK(Encode(Paged(*Find("ANSI_932")), "\uf8f3") == "\xff");
  }

  TEST_CASE("encoding: a copy of a double byte page writes its pairs")
  {
    const Codepage japanese = *Find("ANSI_932");
    CHECK(Encode(Paged(japanese), "\u3042") == "\x82\xa0");
  }
} // namespace DxfIO::Encoding
