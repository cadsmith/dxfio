# C++ Guidelines

## Rules

- Functions and methods must not accept more than 3 parameters.
- Each class must have a single, well-defined responsibility.
- Each file must be dedicated to a single responsibility.
- File names must consist of a single word describing their job.
- File names must be lowercase.
- Directory hierarchies must be used instead of compound file names.
- Namespaces must reflect the directory hierarchy.
- Private member variables must be prefixed with `m_`.
- Namespaces, types, functions, members and constants must use PascalCase.
- Arguments and local variables must use camelCase.
- Template type parameters must end in `T`.
- Public types must take the name DXF gives what they hold: `MText`,
  `LwPolyline`, `Vport`, `BlockRecord`. Functions are named for what they do.
- Every public declaration must have a `///` comment.

## Examples

```cpp
namespace Foo
{
  void Foo(Bar bar);
  void Foo(Bar bar, Baz baz);
  void Foo(Bar bar, Baz baz, Qux qux);

  class Bar
  {
  public:
    bool IsBar();

  private:
    bool m_Bar;
  };
}

namespace Foo::Bar
{
  class Baz
  {
  public:
    bool IsBaz();

  private:
    bool m_Baz;
  };
}
```

```txt
foo/bar.hpp
foo/baz.hpp

foo/bar/baz.hpp
foo/bar/qux.hpp
```
