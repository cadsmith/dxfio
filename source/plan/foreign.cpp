#include "plan/foreign.hpp"

#include "audit/walk.hpp"
#include "header/io.hpp"
#include "objects/io.hpp"
#include "unknown/io.hpp"

#include "dxfio/unknown.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <string>
#include <utility>
#include <variant>

namespace DxfIO::Plan
{
  namespace
  {
    auto Gone(const std::vector<Handle> &gone, Handle one) -> bool
    {
      return one && std::ranges::binary_search(gone, one);
    }

    /**
     *  Whether an object goes: one dxfio does not type, or one owned by an object that goes, as the
     *  extension dictionary of one.
     */
    auto Leaving(const Object &one, const std::vector<Handle> &gone) -> bool
    {
      return std::holds_alternative<Unknown>(one) || Gone(gone, Objects::Based(one).Owner);
    }

    /**
     *  The handles of the objects that go, found again until no more objects are owned by one.
     */
    auto Left(std::span<const Object> all) -> std::vector<Handle>
    {
      std::vector<Handle> out;
      bool                grew = true;
      while (grew)
      {
        const std::size_t before = out.size();
        for (const Object &one : all)
        {
          const Handle self = Objects::Based(one).Self;
          if (self && !Gone(out, self) && Leaving(one, out))
          {
            out.insert(std::ranges::upper_bound(out, self), self);
          }
        }
        grew = out.size() != before;
      }
      return out;
    }

    void Prune(Objects::Dictionary &one, const std::vector<Handle> &gone)
    {
      std::erase_if(one.Entries, [&gone](const Objects::Entry &held) -> bool
                    { return Gone(gone, held.Soft) || Gone(gone, held.Hard); });
    }

    /**
     *  Nulls the handles among tags that name an object gone.
     */
    void Unnamed(std::vector<Tag> &tags, const std::vector<Handle> &gone)
    {
      for (Tag &value : tags)
      {
        if (const auto *named = std::get_if<Handle>(&value.Value);
            named != nullptr && Gone(gone, *named))
        {
          value.Value = Handle{};
        }
      }
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
        if (table->Default && Gone(gone, *table->Default))
        {
          table->Default = Handle{};
        }
      }
      else if (auto *record = std::get_if<Objects::XRecord>(&one))
      {
        Unnamed(record->Data, gone);
      }
    }

    /**
     *  Why an object goes: its kind is not typed, or what owns it goes.
     */
    auto Why(const Object &one, Version from) -> std::string
    {
      if (const auto *kept = std::get_if<Unknown>(&one))
      {
        return std::format("{} left out, its groups are in {}'s form", kept->Type.View(),
                           NameOf(from));
      }
      return std::format("{} left out with the object that owns it", Objects::Type(one));
    }

    void Told(const Drawing &drawing, const std::vector<Handle> &gone, std::vector<Issue> *issues)
    {
      const Version from = Header::Declared(drawing.Header);
      for (const Object &one : drawing.Objects)
      {
        const Handle self = Objects::Based(one).Self;
        if (Leaving(one, gone))
        {
          issues->push_back(
              Issue{.Code = IssueCode::NotKept, .Subject = self, .What = Why(one, from)});
        }
      }
    }
  } // namespace

  void Foreign(Drawing &drawing, std::vector<Issue> *issues)
  {
    const std::vector<Handle> gone = Left(drawing.Objects);
    if (issues != nullptr)
    {
      Told(drawing, gone, issues);
    }
    std::erase_if(drawing.Objects,
                  [&gone](const Object &one) -> bool { return Leaving(one, gone); });
    for (Header::Variable &one : drawing.Header)
    {
      Unnamed(one.Values, gone);
    }
    for (Object &one : drawing.Objects)
    {
      Pruned(one, gone);
    }
    Audit::Bases(drawing,
                 [&gone](Base &base) -> void
                 {
                   std::erase_if(base.Reactors,
                                 [&gone](Handle one) -> bool { return Gone(gone, one); });
                   if (Gone(gone, base.Dictionary))
                   {
                     base.Dictionary = {};
                   }
                 });
  }
} // namespace DxfIO::Plan
