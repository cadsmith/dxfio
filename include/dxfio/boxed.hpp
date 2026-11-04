#ifndef DXFIO_BOXED_HPP
#define DXFIO_BOXED_HPP

#include <memory>
#include <utility>

namespace DxfIO
{
/// A part of an entity kept on the heap, so the Entity variant stays small. It copies and compares as the part.
template <class ValueT>
class Boxed
{
public:
  /// Holds a default part.
  Boxed() : m_Value(std::make_unique<ValueT>()) {}

  /// Holds that part.
  explicit Boxed(ValueT value) : m_Value(std::make_unique<ValueT>(std::move(value))) {}

  /// Holds a copy of the other's part, or none when the other is valueless.
  Boxed(const Boxed &other) : m_Value(Copied(other)) {}

  /// Takes the other's part, leaving the other valueless.
  Boxed(Boxed &&other) noexcept = default;

  /// Copies the other's part into this one, or becomes valueless when the other is.
  auto operator=(const Boxed &other) -> Boxed &
  {
    if (this == &other)
    {
      return *this;
    }
    if (m_Value && other.m_Value)
    {
      *m_Value = *other.m_Value;
      return *this;
    }
    m_Value = Copied(other);
    return *this;
  }

  /// Takes the other's part, leaving the other valueless.
  auto operator=(Boxed &&other) noexcept -> Boxed & = default;

  /// Frees the part.
  ~Boxed() = default;

  /// Whether it holds no part, as after its part was moved out.
  [[nodiscard]] auto Valueless() const noexcept -> bool { return !m_Value; }

  /// The part; a valueless box first takes a default part.
  auto operator*() -> ValueT & { return Held(); }

  /// The part, read only; a valueless box reads as a default part.
  auto operator*() const -> const ValueT & { return m_Value ? *m_Value : Default(); }

  /// Reaches into the part; a valueless box first takes a default part.
  auto operator->() -> ValueT * { return &Held(); }

  /// Reaches into the part, read only; a valueless box reads as a default part.
  auto operator->() const -> const ValueT * { return &**this; }

  /// Whether both hold equal parts, or both are valueless.
  auto operator==(const Boxed &other) const -> bool
  {
    if (!m_Value || !other.m_Value)
    {
      return !m_Value && !other.m_Value;
    }
    return *m_Value == *other.m_Value;
  }

private:
  static auto Copied(const Boxed &other) -> std::unique_ptr<ValueT>
  {
    return other.m_Value ? std::make_unique<ValueT>(*other.m_Value) : nullptr;
  }

  static auto Default() -> const ValueT &
  {
    static const ValueT empty{};
    return empty;
  }

  auto Held() -> ValueT &
  {
    if (!m_Value)
    {
      m_Value = std::make_unique<ValueT>();
    }
    return *m_Value;
  }

  std::unique_ptr<ValueT> m_Value;
};

/// The value itself, so code reaches a member the same way whether it is boxed or not.
template <class ValueT>
auto Unboxed(ValueT &value) -> ValueT &
{
  return value;
}

/// The value itself, read only.
template <class ValueT>
auto Unboxed(const ValueT &value) -> const ValueT &
{
  // NOLINTNEXTLINE(bugprone-return-const-ref-from-parameter)
  return value;
}

/// The part the box holds; a valueless box first takes a default part.
template <class ValueT>
auto Unboxed(Boxed<ValueT> &value) -> ValueT &
{
  return *value;
}

/// The part the box holds, read only; a valueless box reads as a default part.
template <class ValueT>
auto Unboxed(const Boxed<ValueT> &value) -> const ValueT &
{
  return *value;
}
} // namespace DxfIO

#endif /* DXFIO_BOXED_HPP */
