# C++ Guidelines

## Naming

- Namespaces, types, functions, members and constants are PascalCase.
- Parameters and locals are camelCase words; a single letter only for math.
- Private members start with `m_`.
- Template type parameters end in `T`.
- Public types take the name DXF gives what they hold: `MText`, `LwPolyline`,
  `Vport`, `BlockRecord`. Functions are named for what they do.

```cpp
namespace Foo
{
  template <typename BarT>
  class Baz
  {
  public:
    auto Qux(int quux) -> int;

  private:
    int m_Corge{};
  };
} // namespace Foo
```

## Files

- A file holds one responsibility and is named by one lowercase word.
- A directory replaces a compound name; namespaces follow directories.

```txt
foo/bar.hpp      Foo::Bar
foo/baz.hpp      Foo::Baz
foo/bar/qux.hpp  Foo::Bar::Qux
```

## Functions and types

- A function takes at most three parameters.
- A class has one responsibility.

## Layout

- A line ends by the 100th column.
- A namespace indents its body.
- Consecutive declarations align on their names and their `=`.
- Whole numbers in a run right-align.
- A list of struct rows takes one row a line, its fields in columns.
- A broken condition takes one operand a line, the operator leading, aligned on
  its comparisons.
- Consecutive assertions align on their `==`.

```cpp
namespace Foo
{
  auto Bar(const Baz &baz, Qux qux) -> bool
  {
    const int    corge  =     -1;
    const int    grault = 20'000;
    const double garply = 0.5;

    return baz.Low.Fred <= qux.Fred
        && qux.Fred     <= baz.High.Fred
        && baz.Low.Thud <= qux.Thud
        && qux.Thud     <= baz.High.Thud;
  }

  constexpr std::array Waldos{
      Waldo{.Fred =    0, .Thud =     1},
      Waldo{.Fred = -5e6, .Thud = 1e-20},
      Waldo{.Fred =  1e8, .Thud =     2},
  };
} // namespace Foo
```

```cpp
CHECK(Foo(1)    == Bar);
CHECK(Foo(22)   == Baz);
CHECK(Foo(-333) == Qux);
```

## Comments

- Every public declaration has a doc block.
- A comment is a block: `/** */`, its text two spaces in. A run of `//` or
  `///` lines becomes one.
- A member or an enumerator takes a trailing `/**< */` note.
- A `// NOLINT` directive stays as it is.

```cpp
namespace Foo
{
  /**
   *  A bar.
   *
   *  A baz and a qux, held together.
   */
  struct Bar
  {
    int Baz{};     /**< The baz.             */
    int QuxQuux{}; /**< The qux of the quux. */
  };
} // namespace Foo
```

## Tooling

Nothing above is kept by hand. clang-format lays out the code, then
`tools/format/style.py` turns every comment into a block and aligns what
clang-format cannot. CI fails on a file either would change.

```sh
python3 -I tools/format/style.py
python3 -I tools/format/style.py --check
```
