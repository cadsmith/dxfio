#include "tags/decimal.hpp"

#include <doctest/doctest.h>

#include <cmath>
#include <optional>

namespace DxfIO::Tags
{
  TEST_CASE("decimal: a short decimal is read exactly")
  {
    CHECK(Decimal("1.5")      == 1.5);
    CHECK(Decimal("-0.25")    == -0.25);
    CHECK(Decimal("0.1")      == 0.1);
    CHECK(Decimal("100")      == 100.0);
    CHECK(Decimal("0.000123") == 0.000123);
    CHECK(Decimal(".5")       == 0.5);
    CHECK(Decimal("5.")       == 5.0);
    CHECK(Decimal("3.141592653589793")           == 3.141592653589793);
    CHECK(Decimal("9007199254740992")            == 9007199254740992.0);
    CHECK(Decimal("0000000000000000000000001.5") == 1.5);
    CHECK(Decimal("-0").transform([](double value) { return std::signbit(value); }) == true);
  }

  TEST_CASE("decimal: an exponent is read in either case and with either sign")
  {
    CHECK(Decimal("1.234567890123E+20") == 1.234567890123E+20);
    CHECK(Decimal("2.5e3")   == 2500.0);
    CHECK(Decimal("7.0E-10") == 7.0E-10);
    CHECK(Decimal("1e22")    == 1e22);
    CHECK(Decimal("1e-22")   == 1e-22);
    CHECK(Decimal("1e0000000000000000000022") == 1e22);
  }

  TEST_CASE("decimal: a long or far decimal is read exactly too")
  {
    CHECK(Decimal("9007199254740993")            == 9007199254740992.0);
    CHECK(Decimal("12345678901234567890123")     == 12345678901234567890123.0);
    CHECK(Decimal("1.0000000000000000000000001") == 1.0);
    CHECK(Decimal("1e23")  == 1e23);
    CHECK(Decimal("1e-23") == 1e-23);
    CHECK(Decimal("1.7976931348623157e308")  == 1.7976931348623157e308);
    CHECK(Decimal("2.2250738585072011e-308") == 2.2250738585072011e-308);
    CHECK(Decimal("4.9e-324")      == 4.9e-324);
    CHECK(Decimal("0e99999999999") == 0.0);
  }

  TEST_CASE("decimal: anything but a whole decimal is not one")
  {
    CHECK_FALSE(Decimal(""));
    CHECK_FALSE(Decimal("-"));
    CHECK_FALSE(Decimal("."));
    CHECK_FALSE(Decimal("-."));
    CHECK_FALSE(Decimal("e5"));
    CHECK_FALSE(Decimal("1e"));
    CHECK_FALSE(Decimal("1e+"));
    CHECK_FALSE(Decimal("1.5x"));
    CHECK_FALSE(Decimal("1.5.5"));
    CHECK_FALSE(Decimal("1e5.0"));
    CHECK_FALSE(Decimal("123456789012345678901x"));
    CHECK_FALSE(Decimal("+1"));
    CHECK_FALSE(Decimal(" 1"));
    CHECK_FALSE(Decimal("1 "));
    CHECK_FALSE(Decimal("0x1p3"));
  }

  TEST_CASE("decimal: an infinity, a NaN or a value beyond a double is not read")
  {
    CHECK_FALSE(Decimal("inf"));
    CHECK_FALSE(Decimal("-infinity"));
    CHECK_FALSE(Decimal("nan"));
    CHECK_FALSE(Decimal("-NaN"));
    CHECK_FALSE(Decimal("1e400"));
    CHECK_FALSE(Decimal("-1e99999999999"));
    CHECK_FALSE(Decimal("1e-400"));
  }
} // namespace DxfIO::Tags
