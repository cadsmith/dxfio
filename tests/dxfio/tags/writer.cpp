#include "tags/writer.hpp"

#include "../alike.hpp"
#include "../disk.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Tags
{
namespace
{
auto Written(const std::vector<Tag> &all) -> std::string
{
  std::string out;
  Writer to{out};
  for (const Tag &one : all)
  {
    REQUIRE(to.Put(one));
  }
  return out;
}

auto Repeated(std::string_view text, std::size_t times) -> std::string
{
  std::string out;
  for (std::size_t at = 0; at < times; ++at)
  {
    out += text;
  }
  return out;
}
} // namespace

TEST_CASE("writer: tags laid out in AutoCAD's columns")
{
  CHECK(Written({
            Tag{.Code = 0, .Value = std::string_view{"SECTION"}},
            Tag{.Code = 70, .Value = std::int64_t{104}},
            Tag{.Code = 90, .Value = std::int64_t{-5}},
            Tag{.Code = 160, .Value = std::int64_t{7}},
            Tag{.Code = 40, .Value = 1e20},
            Tag{.Code = 290, .Value = true},
            Tag{.Code = 5, .Value = std::string_view{"2C20A"}},
            Tag{.Code = 1, .Value = std::string_view{}},
        }) == "  0\r\nSECTION\r\n 70\r\n   104\r\n 90\r\n       -5\r\n"
              "160\r\n                 7\r\n 40\r\n1.000000000000000E+20\r\n"
              "290\r\n     1\r\n  5\r\n2C20A\r\n  1\r\n\r\n");
}

TEST_CASE("writer: an integer as wide as its column takes no padding")
{
  CHECK(Written({Tag{.Code = 160, .Value = std::int64_t{100000000000000000}}}) == "160\r\n100000000000000000\r\n");
  CHECK(Written({Tag{.Code = 70, .Value = std::int64_t{-32768}}}) == " 70\r\n-32768\r\n");
}

TEST_CASE("writer: a chunk goes out as hex, 127 bytes a tag")
{
  const std::string bytes(130, '\xab');
  CHECK(Written({Tag{.Code = 310, .Value = Chunk{bytes}}}) ==
        "310\r\n" + Repeated("AB", 127) + "\r\n310\r\nABABAB\r\n");
  CHECK(Written({Tag{.Code = 1004, .Value = Chunk{}}}) == "1004\r\n\r\n");
  CHECK(Written({Tag{.Code = 310, .Value = std::string_view{"0102"}}}) == "310\r\n0102\r\n");
}

TEST_CASE("writer: a real that is not finite is a failure, nothing written")
{
  std::string out;
  Writer to{out};
  REQUIRE(to.Put(Tag{.Code = 0, .Value = std::string_view{"SECTION"}}));
  CHECK(to.Put(Tag{.Code = 40, .Value = std::numeric_limits<double>::infinity()}) ==
        std::unexpected{Failure{4, "group 40 holds no finite number"}});
  CHECK(out == "  0\r\nSECTION\r\n");
}

TEST_CASE("writer: a file's tags read and written back are its tags")
{
  const std::string_view text = "  0\r\nSECTION\r\n  2\r\nHEADER\r\n  9\r\n$EXTMIN\r\n"
                                " 10\r\n1.000000000000000E+20\r\n 20\r\n-0.25\r\n 30\r\n0.0\r\n"
                                " 70\r\n    -3\r\n 90\r\n   123456\r\n290\r\n     0\r\n"
                                "330\r\n1F\r\n310\r\nDEAD01\r\n  1\r\n  spaced \r\n"
                                "  0\r\nEOF\r\n";
  const auto first = All(text);
  REQUIRE(first);
  const std::string out = Written(*first);
  CHECK(out == text);
  CHECK(All(out) == first);
}

TEST_CASE("writer: every sample drawing is written back tag for tag")
{
  for (const auto &path : Files(std::filesystem::path{DXFIO_FUZZ_DIR} / "corpus"))
  {
    CAPTURE(path.filename().string());
    const std::string text = Bytes(path);
    const auto first = All(text);
    REQUIRE(first);
    const std::string once = Written(*first);
    CHECK(All(once) == first);
  }
}

TEST_CASE("writer: every ASCII drawing AutoCAD wrote comes back byte for byte "
          "after its header, but a real that reads as the same double")
{
  constexpr std::string_view body = "  2\r\nCLASSES\r\n";
  for (const auto &path : Files(Acad()))
  {
    const std::string name = path.filename().string();
    if (name.contains("binary") || name.contains("r12"))
    {
      continue;
    }
    CAPTURE(name);
    const std::string text = Bytes(path);
    const auto first = All(text);
    REQUIRE(first);
    const std::string once = Written(*first);
    REQUIRE(text.contains(body));
    CHECK(Alike(once.substr(once.find(body)), text.substr(text.find(body))));
  }
}

TEST_CASE("writer: a group of any size, a multiple of 256 bytes among them")
{
  for (const std::size_t size : {std::size_t{255}, std::size_t{256}, std::size_t{512}, std::size_t{8192}})
  {
    CAPTURE(size);
    const std::string text(size, 'x');
    CHECK(Written({Tag{.Code = 1, .Value = std::string_view{text}}}) == "  1\r\n" + text + "\r\n");
  }
}

namespace
{
auto Packed(const Tag &one, Form form = Form::Wide) -> std::string
{
  std::string out;
  Writer to{out, form};
  REQUIRE(to.Put(one));
  return out;
}

auto Refused(const Tag &one, Form form = Form::Wide) -> bool
{
  std::string out;
  Writer to{out, form};
  return !to.Put(one).has_value() && out.empty();
}

constexpr std::array Forms{Form::Ascii, Form::Wide};
} // namespace

TEST_CASE("writer: binary, each type in its own width")
{
  using namespace std::string_view_literals;
  CHECK(Packed(Tag{.Code = 1, .Value = "ab"sv}) == "\x01\0ab\0"sv);
  CHECK(Packed(Tag{.Code = 70, .Value = std::int64_t{258}}) == "\x46\0\x02\x01"sv);
  CHECK(Packed(Tag{.Code = 90, .Value = std::int64_t{1}}) == "\x5a\0\x01\0\0\0"sv);
  CHECK(Packed(Tag{.Code = 160, .Value = std::int64_t{1}}) == "\xa0\0\x01\0\0\0\0\0\0\0"sv);
  CHECK(Packed(Tag{.Code = 40, .Value = 1.0}) == "\x28\0\0\0\0\0\0\0\xf0\x3f"sv);
  CHECK(Packed(Tag{.Code = 290, .Value = true}) == "\x22\x01\x01"sv);
  CHECK(Packed(Tag{.Code = 290, .Value = false}) == "\x22\x01\0"sv);
  CHECK(Packed(Tag{.Code = 310, .Value = Chunk{"\xde\xad"sv}}) == "\x36\x01\x02\xde\xad"sv);
}

TEST_CASE("writer: binary, R12's codes of one byte and an escape")
{
  using namespace std::string_view_literals;
  CHECK(Packed(Tag{.Code = 1, .Value = "a"sv}, Form::Narrow) == "\x01"
                                                                "a\0"sv);
  CHECK(Packed(Tag{.Code = 1000, .Value = "a"sv}, Form::Narrow) == "\xff\xe8\x03"
                                                                   "a\0"sv);
}

TEST_CASE("writer: binary, a chunk longer than a group goes in pieces")
{
  const std::string bytes(200, 'x');
  const std::string out = Packed(Tag{.Code = 310, .Value = Chunk{std::string_view{bytes}}});
  CHECK(out.size() == 200 + (2 * 3));
  CHECK(out[2] == 127);
}

TEST_CASE("writer: binary, a number kept as text takes its group's type")
{
  using namespace std::string_view_literals;
  CHECK(Packed(Tag{.Code = 70, .Value = "258"sv}) == "\x46\0\x02\x01"sv);
  CHECK(Packed(Tag{.Code = 40, .Value = "1.0"sv}) == "\x28\0\0\0\0\0\0\0\xf0\x3f"sv);
}

TEST_CASE("writer: a value of another type is converted, in either form")
{
  using namespace std::string_view_literals;
  for (const Form form : Forms)
  {
    CAPTURE(form);
    CHECK(Packed(Tag{.Code = 70, .Value = 2.0}, form) == Packed(Tag{.Code = 70, .Value = std::int64_t{2}}, form));
    CHECK(Packed(Tag{.Code = 70, .Value = true}, form) == Packed(Tag{.Code = 70, .Value = std::int64_t{1}}, form));
    CHECK(Packed(Tag{.Code = 70, .Value = "258"sv}, form) == Packed(Tag{.Code = 70, .Value = std::int64_t{258}}, form));
    CHECK(Packed(Tag{.Code = 290, .Value = std::int64_t{5}}, form) == Packed(Tag{.Code = 290, .Value = true}, form));
    CHECK(Packed(Tag{.Code = 40, .Value = std::int64_t{1}}, form) == Packed(Tag{.Code = 40, .Value = 1.0}, form));
    CHECK(Packed(Tag{.Code = 40, .Value = true}, form) == Packed(Tag{.Code = 40, .Value = 1.0}, form));
    CHECK(Packed(Tag{.Code = 40, .Value = false}, form) == Packed(Tag{.Code = 40, .Value = 0.0}, form));
    CHECK(Packed(Tag{.Code = 40, .Value = "1.0"sv}, form) == Packed(Tag{.Code = 40, .Value = 1.0}, form));
  }
  CHECK(Packed(Tag{.Code = 70, .Value = 2.0}, Form::Ascii) == " 70\r\n     2\r\n");
  CHECK(Packed(Tag{.Code = 70, .Value = "258"sv}, Form::Ascii) == " 70\r\n   258\r\n");
  CHECK(Packed(Tag{.Code = 290, .Value = std::int64_t{5}}, Form::Ascii) == "290\r\n     1\r\n");
  CHECK(Packed(Tag{.Code = 40, .Value = std::int64_t{1}}, Form::Ascii) == " 40\r\n1.0\r\n");
  CHECK(Packed(Tag{.Code = 1, .Value = std::int64_t{7}}, Form::Ascii) == "  1\r\n     7\r\n");
  CHECK(Packed(Tag{.Code = 1, .Value = true}, Form::Ascii) == "  1\r\n     1\r\n");
  CHECK(Packed(Tag{.Code = 1, .Value = 1.5}, Form::Ascii) == "  1\r\n1.5\r\n");
  CHECK(Packed(Tag{.Code = 1, .Value = Chunk{"\xde"sv}}, Form::Ascii) == "  1\r\nDE\r\n");
  CHECK(Packed(Tag{.Code = 5, .Value = std::int64_t{42}}) == "\x05\0"
                                                             "42\0"sv);
  CHECK(Packed(Tag{.Code = 70, .Value = 2.0}) == "\x46\0\x02\0"sv);
  CHECK(Packed(Tag{.Code = 70, .Value = true}) == "\x46\0\x01\0"sv);
  CHECK(Packed(Tag{.Code = 40, .Value = std::int64_t{1}}) == Packed(Tag{.Code = 40, .Value = 1.0}));
  CHECK(Packed(Tag{.Code = 40, .Value = true}) == Packed(Tag{.Code = 40, .Value = 1.0}));
  CHECK(Packed(Tag{.Code = 40, .Value = false}) == Packed(Tag{.Code = 40, .Value = 0.0}));
  CHECK(Packed(Tag{.Code = 1, .Value = std::int64_t{7}}) == "\x01\0"
                                                            "7\0"sv);
  CHECK(Packed(Tag{.Code = 1, .Value = true}) == "\x01\0"
                                                 "1\0"sv);
  CHECK(Packed(Tag{.Code = 1, .Value = false}) == "\x01\0"
                                                  "0\0"sv);
  CHECK(Packed(Tag{.Code = 1, .Value = 1.5}) == "\x01\0"
                                                "1.5\0"sv);
  CHECK(Packed(Tag{.Code = 1, .Value = Chunk{"\xde"sv}}) == "\x01\0"
                                                            "DE\0"sv);
}

TEST_CASE("writer: what its group cannot hold is a failure, in either form")
{
  using namespace std::string_view_literals;
  for (const Form form : Forms)
  {
    CAPTURE(form);
    CHECK(Refused(Tag{.Code = 70, .Value = "walls"sv}, form));
    CHECK(Refused(Tag{.Code = 40, .Value = "walls"sv}, form));
    CHECK(Refused(Tag{.Code = 310, .Value = std::int64_t{1}}, form));
    CHECK(Refused(Tag{.Code = 70, .Value = std::int64_t{70000}}, form));
    CHECK(Refused(Tag{.Code = 70, .Value = std::int64_t{-70000}}, form));
    CHECK(Refused(Tag{.Code = 90, .Value = std::int64_t{1LL << 40}}, form));
    CHECK(Refused(Tag{.Code = 70, .Value = 1e30}, form));
    CHECK(Refused(Tag{.Code = 70, .Value = Chunk{"a"sv}}, form));
    CHECK(Refused(Tag{.Code = 40, .Value = Chunk{"a"sv}}, form));
    CHECK(Refused(Tag{.Code = 40, .Value = std::numeric_limits<double>::infinity()}, form));
    CHECK(Refused(Tag{.Code = 1, .Value = std::numeric_limits<double>::quiet_NaN()}, form));
  }
}

TEST_CASE("writer: binary, bytes kept as text cannot be packed")
{
  using namespace std::string_view_literals;
  CHECK(Packed(Tag{.Code = 310, .Value = "ZZ"sv}, Form::Ascii) == "310\r\nZZ\r\n");
  CHECK(Refused(Tag{.Code = 310, .Value = "ZZ"sv}));
}

TEST_CASE("writer: a 16 bit group holds -32768 to 65535 in either form, its low 16 bits in binary")
{
  using namespace std::string_view_literals;
  CHECK(Packed(Tag{.Code = 1070, .Value = std::int64_t{40000}}, Form::Ascii) == "1070\r\n 40000\r\n");
  CHECK(Packed(Tag{.Code = 70, .Value = std::int64_t{-32768}}, Form::Ascii) == " 70\r\n-32768\r\n");
  CHECK(Packed(Tag{.Code = 1070, .Value = std::int64_t{40000}}) == "\x2e\x04\x40\x9c"sv);
  CHECK(Packed(Tag{.Code = 70, .Value = std::int64_t{65535}}) == "\x46\0\xff\xff"sv);
  CHECK(Refused(Tag{.Code = 70, .Value = std::int64_t{65536}}, Form::Ascii));
  CHECK(Refused(Tag{.Code = 70, .Value = std::int64_t{-32769}}));
}

TEST_CASE("writer: a 16 bit value read from ASCII above 32767 saves as binary")
{
  const auto read = All("1070\n40000\n");
  REQUIRE(read);
  CHECK(Packed(read->front()) == Packed(Tag{.Code = 1070, .Value = std::int64_t{40000}}));
}

TEST_CASE("writer: binary, a 64 bit group holds what 32 bits cannot")
{
  CHECK(Packed(Tag{.Code = 160, .Value = std::int64_t{1LL << 40}}).size() == 10);
}
} // namespace DxfIO::Tags

namespace DxfIO::Tags
{
TEST_CASE("writer: a real too large for a whole number's group is refused")
{
  std::string out;
  Writer to{out, Form::Wide};
  CHECK_FALSE(to.Put(Tag{.Code = 70, .Value = 1e19}));
  CHECK(to.Put(Tag{.Code = 70, .Value = 7.0}));
}
} // namespace DxfIO::Tags

namespace DxfIO::Tags
{
TEST_CASE("writer: a real not finite in a whole number's group is refused")
{
  std::string out;
  Writer to{out, Form::Wide};
  CHECK_FALSE(to.Put(Tag{.Code = 70, .Value = std::numeric_limits<double>::infinity()}));
}
} // namespace DxfIO::Tags
