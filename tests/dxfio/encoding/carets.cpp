#include "encoding/carets.hpp"

#include <doctest/doctest.h>

#include <string>

namespace DxfIO::Encoding
{
TEST_CASE("carets: a pair stands for a control character or a caret")
{
  CHECK(Caret("^J") == '\n');
  CHECK(Caret("^M") == '\r');
  CHECK(Caret("^I") == '\t');
  CHECK(Caret("^@") == '\0');
  CHECK(Caret("^ 2") == '^');
}

TEST_CASE("carets: a caret not in a pair is no pair")
{
  CHECK_FALSE(Caret("^2"));
  CHECK_FALSE(Caret("^"));
  CHECK_FALSE(Caret("a^J"));
}

TEST_CASE("carets: a control character or a caret appended as its pair")
{
  std::string out;
  for (const char c : std::string{"a\nb\r\tc^d"})
  {
    Careted(out, c);
  }
  CHECK(out == "a^Jb^M^Ic^ d");
}
} // namespace DxfIO::Encoding

namespace DxfIO::Encoding
{
TEST_CASE("carets: a caret before a small letter is no pair")
{
  CHECK_FALSE(Caret("^a"));
}
} // namespace DxfIO::Encoding
