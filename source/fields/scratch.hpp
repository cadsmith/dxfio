#ifndef DXFIO_FIELDS_SCRATCH_HPP
#define DXFIO_FIELDS_SCRATCH_HPP

#include "fields/context.hpp"
#include "fields/repeats.hpp"
#include "tags/tag.hpp"

#include "dxfio/api.hpp"

#include <utility>
#include <vector>

namespace DxfIO::Fields
{
  /**
   *  What reading a record needs for itself alone, kept from one record to the next so the room is
   *  found once: the tags its head leaves, a walk's used and unfit flags, and the codes its fields
   *  asked for. A walk borrows them while it lives and gives them back; while it holds them,
   *  another walk makes its own.
   */
  struct DXFIO_API Scratch
  {
    std::vector<Tags::Tag> Rest;
    std::vector<bool>      Used;
    std::vector<bool>      Unfit;
    Repeats                Asked;
    bool                   Busy{};
  };

  /**
   *  A walk's hold on a scratch. A copy of the walk holds none and keeps its own room; a move
   *  carries the hold over, and a walk assigned to keeps the hold it has, taking the other's only
   *  when it has none.
   */
  class DXFIO_API Lease
  {
  public:
    Lease() = default;

    explicit Lease(Scratch *from) : m_From(from) {}

    Lease(const Lease & /*unused*/) {}

    Lease(Lease &&other) noexcept : m_From(std::exchange(other.m_From, nullptr)) {}

    auto operator=(const Lease &other) -> Lease &
    {
      if (this != &other)
      {
        Taken(Lease{other});
      }
      return *this;
    }

    auto operator=(Lease &&other) noexcept -> Lease &
    {
      if (this != &other)
      {
        Taken(std::move(other));
      }
      return *this;
    }

    ~Lease() = default;

    [[nodiscard]] auto From() const -> Scratch * { return m_From; }

  private:
    void Taken(Lease other) noexcept
    {
      if (m_From == nullptr)
      {
        std::swap(m_From, other.m_From);
      }
    }

    Scratch *m_From{nullptr};
  };

  /**
   *  Gives a head's rest back to the scratch, when one is given, for the next record's.
   */
  inline void Returned(const Reading &with, std::vector<Tags::Tag> &&rest)
  {
    if (with.Reuse != nullptr)
    {
      with.Reuse->Rest = std::move(rest);
    }
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_SCRATCH_HPP */
