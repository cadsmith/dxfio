#include "plan/foreign.hpp"

#include "objects/io.hpp"
#include "unknown/io.hpp"

#include "dxfio/unknown.hpp"

#include <algorithm>
#include <variant>

namespace DxfIO::Plan
{
  namespace
  {
    auto Left(std::span<const Object> all) -> std::vector<Handle>
    {
      std::vector<Handle> out;
      for (const Object &one : all)
      {
        const Handle self = Objects::Based(one).Self;
        if (self && std::holds_alternative<Unknown>(one))
        {
          out.push_back(self);
        }
      }
      std::ranges::sort(out);
      return out;
    }

    void Prune(Objects::Dictionary &one, const std::vector<Handle> &gone)
    {
      std::erase_if(one.Entries,
                    [&gone](const Objects::Entry &held) -> bool
                    {
                      return std::ranges::binary_search(gone, held.Soft)
                          || std::ranges::binary_search(gone, held.Hard);
                    });
    }
  } // namespace

  auto Foreign(std::vector<Object> all) -> std::vector<Object>
  {
    const std::vector<Handle> gone = Left(all);
    std::erase_if(all,
                  [](const Object &one) -> bool { return std::holds_alternative<Unknown>(one); });
    for (Object &one : all)
    {
      if (auto *table = std::get_if<Objects::Dictionary>(&one))
      {
        Prune(*table, gone);
      }
    }
    return all;
  }
} // namespace DxfIO::Plan
