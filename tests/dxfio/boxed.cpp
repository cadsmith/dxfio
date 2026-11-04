#include "dxfio/boxed.hpp"

#include <doctest/doctest.h>

#include <string>
#include <utility>

namespace DxfIO
{
TEST_CASE("boxed: copied deep, compared by what it holds")
{
  Boxed<std::string> one{std::string{"walls"}};
  Boxed<std::string> copy = one;
  CHECK(copy == one);
  CHECK(&*copy != &*one);
  *copy = "doors";
  CHECK(*one == "walls");
  CHECK_FALSE(copy == one);
}

TEST_CASE("boxed: moved by its pointer")
{
  Boxed<std::string> one{std::string{"walls"}};
  const std::string *held = &*one;
  const Boxed<std::string> moved = std::move(one);
  CHECK(&*moved == held);
}

TEST_CASE("boxed: moved from, it copies and compares as holding nothing")
{
  Boxed<std::string> one{std::string{"walls"}};
  Boxed<std::string> moved = std::move(one);
  // NOLINTBEGIN(bugprone-use-after-move,clang-analyzer-cplusplus.Move)
  CHECK(one.Valueless());
  const Boxed<std::string> copy = one;
  CHECK(copy.Valueless());
  CHECK(copy == one);
  CHECK_FALSE(copy == moved);
  one = moved;
  // NOLINTEND(bugprone-use-after-move,clang-analyzer-cplusplus.Move)
  CHECK_FALSE(one.Valueless());
  CHECK(one == moved);
}

TEST_CASE("boxed: assigned a copy, it keeps its own storage")
{
  Boxed<std::string> one{std::string{"walls"}};
  const Boxed<std::string> other{std::string{"doors"}};
  const std::string *held = &*one;
  one = other;
  CHECK(&*one == held);
  CHECK(*one == "doors");
}

TEST_CASE("boxed: unboxed hands out the value, held in a box or not")
{
  Boxed<int> box{3};
  int plain = 4;
  CHECK(Unboxed(box) == 3);
  CHECK(Unboxed(plain) == 4);
  Unboxed(box) = 5;
  CHECK(*box == 5);
}

TEST_CASE("boxed: assigned to itself it keeps what it holds")
{
  Boxed<std::string> one{std::string{"walls"}};
  Boxed<std::string> &same = one;
  one = same;
  CHECK(*one == "walls");
}

TEST_CASE("boxed: move assignment takes the pointer")
{
  Boxed<std::string> one{std::string{"walls"}};
  Boxed<std::string> other{std::string{"doors"}};
  const std::string *held = &*one;
  other = std::move(one);
  CHECK(&*other == held);
}
} // namespace DxfIO

namespace DxfIO
{
TEST_CASE("boxed: one holding a value against one moved from")
{
  Boxed<std::string> full{std::string{"walls"}};
  Boxed<std::string> source{std::string{"doors"}};
  const Boxed<std::string> taken = std::move(source);
  // NOLINTBEGIN(bugprone-use-after-move,clang-analyzer-cplusplus.Move)
  CHECK_FALSE(full == source);
  full = source;
  // NOLINTEND(bugprone-use-after-move,clang-analyzer-cplusplus.Move)
  CHECK(full.Valueless());
  CHECK_FALSE(taken.Valueless());
}

TEST_CASE("boxed: moved from, it reads as a default part and takes a new one when written")
{
  Boxed<std::string> one{std::string{"walls"}};
  Boxed<std::string> two{std::string{"doors"}};
  const Boxed<std::string> first = std::move(one);
  const Boxed<std::string> second = std::move(two);
  // NOLINTBEGIN(bugprone-use-after-move,clang-analyzer-cplusplus.Move)
  const Boxed<std::string> &seen = one;
  CHECK(seen->empty());
  CHECK((*seen).empty());
  CHECK(seen.Valueless());
  one->append("roof");
  *two = "floor";
  // NOLINTEND(bugprone-use-after-move,clang-analyzer-cplusplus.Move)
  CHECK(*one == "roof");
  CHECK(*two == "floor");
  CHECK(*first == "walls");
  CHECK(*second == "doors");
}
} // namespace DxfIO
