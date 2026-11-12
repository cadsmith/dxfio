#include "encoding/codepage.hpp"

#include <doctest/doctest.h>

namespace DxfIO::Encoding
{
  TEST_CASE("codepage: a page found by its header name, whatever its case")
  {
    REQUIRE(Find("ANSI_1256") != nullptr);
    CHECK(Find("ansi_1256") == Find("ANSI_1256"));
    CHECK(Find("1251")      == Find("ANSI_1251"));
    CHECK(Find("ANSI_874") != nullptr);
    CHECK(Pages().size()  == 15);
    CHECK(Standard().Name == "ANSI_1252");
  }

  TEST_CASE("codepage: the DWG names of the East Asian pages find them, whatever their case")
  {
    CHECK(Find("ksc5601")   == Find("ANSI_949"));
    CHECK(Find("KSC5601")   == Find("ANSI_949"));
    CHECK(Find("big5")      == Find("ANSI_950"));
    CHECK(Find("johab")     == Find("ANSI_1361"));
    CHECK(Find("gb2312")    == Find("ANSI_936"));
    CHECK(Find("dos932")    == Find("ANSI_932"));
    CHECK(Find("ANSI_big5") == nullptr);
  }

  TEST_CASE("codepage: a page not carried is none")
  {
    CHECK(Find("ANSI_1200") == nullptr);
    CHECK(Find("DOS850")    == nullptr);
    CHECK(Find("")          == nullptr);
    CHECK(Find("ANSI_")     == nullptr);
    CHECK(Find("ANSI")      == nullptr);
    CHECK(Find("ANSI_ANSI_932") == nullptr);
    CHECK(Find("ANSI_93")       == nullptr);
  }

  TEST_CASE("codepage: the prefix and the number matched without case")
  {
    REQUIRE(Find("ANSI_932") != nullptr);
    CHECK(Find("Ansi_932")        == Find("ANSI_932"));
    CHECK(Find("932")             == Find("ANSI_932"));
    CHECK(Find("ANSI_1361")->Name == "ANSI_1361");
  }

  TEST_CASE("codepage: a byte's point and a point's byte")
  {
    const Codepage &latin  = Standard();
    const Codepage &arabic = *Find("ANSI_1256");
    CHECK(Decoded(latin, 'a')   == U'a');
    CHECK(Decoded(latin, 0xe9)  == U'é');
    CHECK(Decoded(latin, 0x80)  == U'€');
    CHECK(Decoded(latin, 0x81)  == U'\u0081');
    CHECK(Decoded(arabic, 0xc7) == U'ا');
    CHECK(Encoded(latin, U'é')  == '\xe9');
    CHECK(Encoded(arabic, U'ا') == '\xc7');
    CHECK_FALSE(Encoded(latin, U'ا'));
  }

  TEST_CASE("codepage: the East Asian pages, two bytes a character")
  {
    const Codepage &japanese = *Find("ANSI_932");
    CHECK(Paired(japanese, 0x82, 0xa0) == U'\u3042');
    CHECK(Paired(japanese, 0x81, 0x5c) == U'\u2015');
    CHECK(Decoded(japanese, 0xb1)      == U'\uff71');
    CHECK(Paired(*Find("ANSI_936"), 0xd6, 0xd0)  == U'\u4e2d');
    CHECK(Paired(*Find("ANSI_949"), 0xb0, 0xa1)  == U'\uac00');
    CHECK(Paired(*Find("ANSI_950"), 0xa4, 0xa4)  == U'\u4e2d');
    CHECK(Paired(*Find("ANSI_1361"), 0x88, 0x61) == U'\uac00');
    CHECK(Doubled(japanese, U'\u3042')           == Twin{'\x82', '\xa0'});
    CHECK(Doubled(*Find("ANSI_936"), U'\u4e2d')  == Twin{'\xd6', '\xd0'});
  }

  TEST_CASE("codepage: no pair outside the table, nor in a page of one byte")
  {
    const Codepage &japanese = *Find("ANSI_932");
    CHECK(Paired(Standard(), 0x82, 0xa0) == 0);
    CHECK(Paired(japanese, 0x41, 0xa0)   == 0);
    CHECK(Paired(japanese, 0x82, 0x20)   == 0);
    CHECK(Paired(japanese, 0xff, 0xa0)   == 0);
    CHECK(Paired(japanese, 0x82, 0xff)   == 0);
    CHECK(Paired(japanese, 0x82, 0x30)   == 0);
    CHECK_FALSE(Doubled(japanese, U'\u0627'));
    CHECK_FALSE(Doubled(japanese, U'\U0001F600'));
    CHECK_FALSE(Doubled(Standard(), U'\u3042'));
  }

  TEST_CASE("codepage: a copy of a page pairs as the page does")
  {
    const Codepage japanese = *Find("ANSI_932");
    const Codepage chinese  = *Find("ANSI_936");
    CHECK(Doubled(japanese, U'あ') == Twin{'\x82', '\xa0'});
    CHECK(Doubled(chinese, U'中')  == Twin{'\xd6', '\xd0'});
    CHECK_FALSE(Doubled(Standard(), U'中'));
  }

  TEST_CASE("codepage: a point below 128 is its own byte in every page")
  {
    CHECK(Encoded(Standard(), U'A')            == 'A');
    CHECK(Encoded(*Find("ANSI_1256"), U'\x7f') == '\x7f');
  }
} // namespace DxfIO::Encoding
