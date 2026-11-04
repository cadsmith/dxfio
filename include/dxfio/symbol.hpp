#ifndef DXFIO_SYMBOL_HPP
#define DXFIO_SYMBOL_HPP

#include "dxfio/api.hpp"

#include <cstddef>
#include <format>
#include <functional>
#include <string>
#include <string_view>

namespace DxfIO
{
/// A name records repeat, such as a layer, linetype, text style or block, stored once for the whole program.
/// Symbols of the same text share it, so a copy costs a pointer and comparing two compares addresses.
class DXFIO_API Symbol
{
public:
  /// The empty name.
  Symbol();
  /// The name with that text, case kept. A text not seen before joins a store every thread shares, for good.
  explicit Symbol(std::string_view text);
  /// The name with that text.
  explicit Symbol(const std::string &text);
  /// The name with that text, which must not be null.
  explicit Symbol(const char *text);

  /// The text, valid for as long as the program runs.
  [[nodiscard]] auto View() const -> std::string_view;
  /// The text as a string, valid for as long as the program runs.
  [[nodiscard]] auto Str() const -> const std::string &;
  /// Whether the text has no characters.
  [[nodiscard]] auto Empty() const -> bool;

  auto operator==(const Symbol &) const -> bool = default;

  /// Whether the name is exactly that text, case included.
  auto operator==(std::string_view text) const -> bool;

private:
  const std::string *m_Text;
};
} // namespace DxfIO

/// Hashes a symbol by the address of its shared text.
template <>
struct std::hash<DxfIO::Symbol>
{
  auto operator()(const DxfIO::Symbol &one) const noexcept -> std::size_t
  {
    return std::hash<const void *>{}(&one.Str());
  }
};

/// Formats a symbol as its text, with the options a string_view takes.
template <>
struct std::formatter<DxfIO::Symbol> : std::formatter<std::string_view>
{
  template <class ContextT>
  auto format(const DxfIO::Symbol &one, ContextT &context) const
  {
    return std::formatter<std::string_view>::format(one.View(), context);
  }
};

#endif /* DXFIO_SYMBOL_HPP */
