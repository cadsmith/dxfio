#include "dxfio/symbol.hpp"

#include "seek.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <mutex>
#include <unordered_set>

namespace DxfIO
{
  namespace
  {
    struct Hashing
    {
      // NOLINTNEXTLINE(readability-identifier-naming): the name the standard looks for
      using is_transparent = void;

      auto operator()(std::string_view text) const noexcept -> std::size_t
      {
        return std::hash<std::string_view>{}(text);
      }
    };

    struct Pool
    {
      std::mutex                                                Lock;
      std::unordered_set<std::string, Hashing, std::equal_to<>> Texts;
    };

    constexpr std::size_t Remembered = 4;

    /**
     *  The pool in shards, each with a lock of its own, so threads reading a drawing's records
     *  together seldom wait on one another; a text's shard follows its hash.
     */
    constexpr std::size_t Shards = 16;

    auto Shared(std::string_view text) -> Pool &
    {
      static std::array<Pool, Shards> all;
      return all.at(Hashing{}(text) % Shards);
    }

    auto Pooled(std::string_view text) -> const std::string *
    {
      Pool                  &shard = Shared(text);
      const std::scoped_lock guard{shard.Lock};
      const auto             found = shard.Texts.find(text);
      return found == shard.Texts.end() ? &*shard.Texts.emplace(text).first : &*found;
    }

    auto Interned(std::string_view text) -> const std::string *
    {
      static const std::string empty;
      if (text.empty())
      {
        return &empty;
      }
      thread_local std::array<const std::string *, Remembered> recent{};
      const auto *known = SeekIf(recent, [text](const std::string *one) -> bool
                                 { return one != nullptr && *one == text; });
      if (known != nullptr)
      {
        return *known;
      }
      std::ranges::rotate(recent, std::prev(recent.end()));
      recent.front() = Pooled(text);
      return recent.front();
    }
  } // namespace

  Symbol::Symbol() : m_Text(Interned({})) {}

  Symbol::Symbol(std::string_view text) : m_Text(Interned(text)) {}

  Symbol::Symbol(const std::string &text) : m_Text(Interned(text)) {}

  Symbol::Symbol(const char *text) : m_Text(Interned(text)) {}

  auto Symbol::View() const -> std::string_view
  {
    return *m_Text;
  }

  auto Symbol::Str() const -> const std::string &
  {
    return *m_Text;
  }

  auto Symbol::Empty() const -> bool
  {
    return m_Text->empty();
  }

  auto Symbol::operator==(std::string_view text) const -> bool
  {
    return View() == text;
  }
} // namespace DxfIO
