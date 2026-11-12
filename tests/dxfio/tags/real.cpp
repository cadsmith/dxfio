#include "tags/real.hpp"

#include "tags/values.hpp"

#include <doctest/doctest.h>

#include <limits>
#include <string>

namespace DxfIO::Tags
{
  TEST_CASE("real: written as AutoCAD writes it, a point always given")
  {
    CHECK(Real(0.0)      == "0.0");
    CHECK(Real(1.0)      == "1.0");
    CHECK(Real(-0.5)     == "-0.5");
    CHECK(Real(0.1)      == "0.1");
    CHECK(Real(-2500.25) == "-2500.25");
    CHECK(Real(343276.6700000018)   == "343276.6700000018");
    CHECK(Real(0.30000000000000004) == "0.30000000000000004");
  }

  TEST_CASE("real: laid out as AutoCAD's own files lay it out")
  {
    CHECK(Real(100000.0) == "100000.0");
    CHECK(Real(1e14)     == "100000000000000.0");
    CHECK(Real(1e15)     == "1000000000000000");
    CHECK(Real(9.5e15)   == "9500000000000000");
    CHECK(Real(1234567890123456.0) == "1234567890123456");
    CHECK(Real(1e16)        == "1.000000000000000E+16");
    CHECK(Real(1.5e16)      == "1.500000000000000E+16");
    CHECK(Real(1e20)        == "1.000000000000000E+20");
    CHECK(Real(0.0001)      == "0.0001");
    CHECK(Real(0.00001)     == "0.00001");
    CHECK(Real(0.000012345) == "0.000012345");
    CHECK(Real(1e-10)       == "0.0000000001");
    CHECK(Real(2e-16)       == "0.0000000000000002");
    CHECK(Real(1e-16)       == "0.0000000000000001");
  }

  TEST_CASE("real: what AutoCAD would lose kept, an exponent where it has none")
  {
    CHECK(Real(1e-17)   == "1.000000000000000E-17");
    CHECK(Real(1.5e-16) == "0.00000000000000015");
    CHECK(Real(-1e-300) == "-1.000000000000000E-300");
    CHECK(Real(1.797693134862313e99)  == "1.797693134862313E+99");
    CHECK(Real(1.0000000000000002)    == "1.0000000000000002");
    CHECK(Real(1.2345678901234567e20) == "1.2345678901234567E+20");
  }

  TEST_CASE("real: what is written reads back as the value")
  {
    for (const double value :
         {0.1, 1.0 / 3.0, 343276.6700000018, 6.02214076e23, -1e-300, 123456.789e-3,
          std::numeric_limits<double>::max(), std::numeric_limits<double>::denorm_min()})
    {
      const std::optional<std::string> text = Real(value);
      REQUIRE(text);
      CHECK(Parse(Codes::Type::Real, *text) == Content{value});
    }
  }

  TEST_CASE("real: an infinity or a nan has no text")
  {
    CHECK_FALSE(Real(std::numeric_limits<double>::infinity()));
    CHECK_FALSE(Real(-std::numeric_limits<double>::infinity()));
    CHECK_FALSE(Real(std::numeric_limits<double>::quiet_NaN()));
  }
} // namespace DxfIO::Tags
