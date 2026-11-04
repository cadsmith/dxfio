#include "empty/roots.hpp"

#include <doctest/doctest.h>

#include <set>

namespace DxfIO::Empty
{
TEST_CASE("roots: each a handle of its own, the seed past them all")
{
  Seed handles;
  const Roots ids = Taken(handles);
  const std::set<Handle> seen{ids.Dictionary,
                              ids.Records.Model,
                              ids.Records.Paper,
                              ids.Layouts.Model,
                              ids.Layouts.Paper,
                              ids.Materials.Byblock,
                              ids.Materials.Bylayer,
                              ids.Materials.Global,
                              ids.Normal,
                              ids.Linetype,
                              ids.Style};
  CHECK(seen.size() == 11);
  CHECK_FALSE(seen.contains(Handle{}));
  CHECK(handles.Next() == Handle{.Value = 12});
}
} // namespace DxfIO::Empty
