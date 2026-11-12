#ifndef DXFIO_FIELDS_WALK_HPP
#define DXFIO_FIELDS_WALK_HPP

#include "fields/budget.hpp"
#include "fields/context.hpp"
#include "fields/repeats.hpp"
#include "fields/scratch.hpp"
#include "tags/tag.hpp"
#include "unknown/io.hpp"

#include "dxfio/api.hpp"
#include "dxfio/group.hpp"
#include "dxfio/issue.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace DxfIO::Fields
{
  class DXFIO_API Walk
  {
  public:
    struct Fence
    {
      std::size_t Begin{};
      std::size_t End{};
      std::size_t Cursor{};
    };

    Walk(std::span<const Tags::Tag> body, const Reading &with);
    Walk(const Walk &)                     = default;
    Walk(Walk &&)                          = default;
    auto operator=(const Walk &) -> Walk & = default;
    auto operator=(Walk &&) -> Walk &      = default;
    ~Walk();

    [[nodiscard]] auto With() const -> const Reading & { return m_With; }

    using Test = bool (*)(const Tags::Tag &one);

    auto Take(int code, Test fits = nullptr) -> const Tags::Tag *;

    auto Subclass(std::string_view marker) -> bool;

    auto Subclass(std::span<const std::string_view> markers) -> bool;

    void Join(std::string_view marker);

    auto Embed() -> bool;

    auto Gather(std::span<const int> codes) -> std::vector<const Tags::Tag *>;

    [[nodiscard]] auto Later(int code, int other) const -> bool;

    void Sequence(std::string_view marker);

    [[nodiscard]] auto Whole() const -> bool;

    void Refuse() { m_Refused = true; }

    [[nodiscard]] auto Holds(int code) const -> bool;

    void Expect(int code);

    void Rewind() { m_Fence.Cursor = m_Fence.Begin; }

    void Narrow(int opener, const Fence &outer);

    [[nodiscard]] auto Fenced() const -> Fence { return m_Fence; }

    void Restore(const Fence &saved);

    auto Claim(std::int64_t wanted) -> std::size_t;

    [[nodiscard]] auto Rest() const -> std::vector<Group>;

    void Spend();

    [[nodiscard]] auto Kept() -> std::vector<Group>;

  private:
    struct Standing
    {
      bool Beyond{};
      bool Early{};
    };

    [[nodiscard]] auto At(std::size_t index) const -> const Tags::Tag &;

    [[nodiscard]] auto Dropped(std::size_t at, int code, Standing where)
        -> std::optional<std::string>;
    void               Judge(int code, Test fits);
    [[nodiscard]] auto Chosen(int code) const -> std::optional<std::size_t>;
    auto               Ordered(int code) -> const Tags::Tag *;
    void               Pass();
    [[nodiscard]] auto Last(int code) const -> std::optional<std::size_t>;
    [[nodiscard]] auto After(int code) const -> std::optional<std::size_t>;
    [[nodiscard]] auto Next(int code, std::size_t from, std::size_t to) const
        -> std::optional<std::size_t>;
    [[nodiscard]] auto Closing(std::size_t from) const -> std::size_t;
    auto               Through(std::size_t from, std::string_view marker) -> std::size_t;
    [[nodiscard]] auto Opening() const -> std::size_t;
    [[nodiscard]] auto Uncovered() const -> std::size_t;

    std::span<const Tags::Tag> m_Body;
    Reading                    m_With;
    std::vector<bool>          m_Used;
    std::vector<bool>          m_Unfit;
    Fence                      m_Fence;
    bool                       m_Marked{};
    std::string_view           m_Joined;
    bool                       m_Strict{};
    bool                       m_Ordered{};
    bool                       m_Refused{};
    std::size_t                m_Next{};
    std::size_t                m_Stop{};
    Fields::Budget             m_Budget;
    Fields::Repeats            m_Repeats;
    Lease                      m_Lease;
  };
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_WALK_HPP */
