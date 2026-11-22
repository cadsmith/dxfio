#include "plan/foreign.hpp"

#include "audit/walk.hpp"
#include "header/io.hpp"
#include "objects/io.hpp"
#include "unknown/io.hpp"

#include "dxfio/unknown.hpp"

#include <algorithm>
#include <format>
#include <utility>
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

    /**
     *  Drops the entries of a dictionary, and the default of one that has it, that name a record
     *  gone.
     */
    void Pruned(Object &one, const std::vector<Handle> &gone)
    {
      if (auto *table = std::get_if<Objects::Dictionary>(&one))
      {
        Prune(*table, gone);
      }
      else if (auto *defaulted = std::get_if<Objects::DictionaryWithDefault>(&one))
      {
        Prune(defaulted->Dictionary, gone);
        if (std::ranges::binary_search(gone, defaulted->Fallback))
        {
          defaulted->Fallback = {};
        }
      }
    }

    void Told(std::span<const Object> all, Version from, std::vector<Issue> *issues)
    {
      for (const Object &one : all)
      {
        if (const auto *kept = std::get_if<Unknown>(&one))
        {
          issues->push_back(Issue{
              .Code    = IssueCode::NotKept,
              .Subject = kept->Base.Self,
              .What    = std::format("{} left out, its groups are in {}'s form", kept->Type.View(),
                                     NameOf(from)),
          });
        }
      }
    }
  } // namespace

  void Foreign(Drawing &drawing, std::vector<Issue> *issues)
  {
    const std::vector<Handle> gone = Left(drawing.Objects);
    if (issues != nullptr)
    {
      Told(drawing.Objects, Header::Declared(drawing.Header), issues);
    }
    std::erase_if(drawing.Objects,
                  [](const Object &one) -> bool { return std::holds_alternative<Unknown>(one); });
    for (Object &one : drawing.Objects)
    {
      Pruned(one, gone);
    }
    Audit::Bases(drawing,
                 [&gone](Base &base) -> void
                 {
                   std::erase_if(base.Reactors, [&gone](Handle one) -> bool
                                 { return std::ranges::binary_search(gone, one); });
                   if (std::ranges::binary_search(gone, base.Dictionary))
                   {
                     base.Dictionary = {};
                   }
                 });
  }
} // namespace DxfIO::Plan
