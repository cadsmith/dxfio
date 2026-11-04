#include "tags/codes.hpp"

#include <doctest/doctest.h>

namespace DxfIO::Tags::Codes
{
TEST_CASE("codes: a code's type follows Autodesk's ranges")
{
  CHECK(Of(0) == Type::Text);
  CHECK(Of(2) == Type::Text);
  CHECK(Of(5) == Type::Handle);
  CHECK(Of(8) == Type::Text);
  CHECK(Of(10) == Type::Real);
  CHECK(Of(39) == Type::Real);
  CHECK(Of(48) == Type::Real);
  CHECK(Of(62) == Type::Int16);
  CHECK(Of(90) == Type::Int32);
  CHECK(Of(100) == Type::Text);
  CHECK(Of(102) == Type::Text);
  CHECK(Of(105) == Type::Handle);
  CHECK(Of(160) == Type::Int64);
  CHECK(Of(210) == Type::Real);
  CHECK(Of(280) == Type::Int16);
  CHECK(Of(290) == Type::Boolean);
  CHECK(Of(310) == Type::Binary);
  CHECK(Of(330) == Type::Handle);
  CHECK(Of(370) == Type::Int16);
  CHECK(Of(390) == Type::Handle);
  CHECK(Of(420) == Type::Int32);
  CHECK(Of(440) == Type::Int32);
  CHECK(Of(450) == Type::Int32);
  CHECK(Of(481) == Type::Handle);
  CHECK(Of(999) == Type::Text);
  CHECK(Of(1001) == Type::Text);
  CHECK(Of(1004) == Type::Binary);
  CHECK(Of(1040) == Type::Real);
  CHECK(Of(1070) == Type::Int16);
  CHECK(Of(1071) == Type::Int32);
}

TEST_CASE("codes: a code outside every range is text")
{
  CHECK(Of(2000) == Type::Text);
  CHECK(Of(-1) == Type::Text);
}
} // namespace DxfIO::Tags::Codes
