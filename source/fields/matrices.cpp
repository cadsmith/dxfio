#include "fields/matrices.hpp"

#include "chained.hpp"
#include "fields/numbers.hpp"

namespace DxfIO::Fields
{
  auto Value<Matrix>::Read(Walk &in, int code) -> std::optional<Matrix>
  {
    Matrix            out;
    const Walk::Fence outer = in.Fenced();
    in.Rewind();
    bool given = false;
    for (double &cell : out.Cells)
    {
      in.Narrow(code, outer);
      const std::optional<double> found = Value<double>::Read(in, code);
      cell                              = found.value_or(cell);
      given                             = given || found.has_value();
    }
    in.Restore(outer);
    return given ? std::optional{out} : std::nullopt;
  }

  auto Value<Matrix>::Write(Writing &to, int code, const Matrix &held)
      -> std::expected<void, Failure>
  {
    return Chained(held.Cells, [&](const double cell) -> std::expected<void, Failure>
                   { return Value<double>::Write(to, code, cell); });
  }
} // namespace DxfIO::Fields
