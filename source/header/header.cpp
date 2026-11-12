#include "dxfio/header/header.hpp"

#include "chained.hpp"
#include "fields/owned.hpp"
#include "fields/text.hpp"
#include "header/created.hpp"
#include "header/io.hpp"
#include "header/rows.hpp"

#include "dxfio/version.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <variant>

namespace DxfIO::Header
{
  namespace
  {
    constexpr int Name = 9;

    constexpr int Primary = 1;

    constexpr int Secondary = 3;

    constexpr std::string_view Acadver = "$ACADVER";

    constexpr std::string_view Codepage = "$DWGCODEPAGE";

    auto Spelled(const Tags::Tag &held) -> std::string_view
    {
      return Tags::Text(held);
    }

    auto Spelled(const Group &held) -> std::string_view
    {
      const auto *text = std::get_if<std::string>(&held.Value);
      return text == nullptr ? std::string_view{} : std::string_view{*text};
    }

    template <class VariableT>
    auto Valued(std::span<const VariableT> all, std::string_view wanted, int code)
        -> std::string_view
    {
      const auto found = std::ranges::find(all, wanted, &VariableT::Name);
      if (found == all.end())
      {
        return {};
      }
      const auto held = std::ranges::find_if(found->Values, [code](const auto &one) -> bool
                                             { return one.Code == code; });
      return held == found->Values.end() ? std::string_view{} : Spelled(*held);
    }

    auto Paged(std::string_view text) -> const Encoding::Codepage &
    {
      const Encoding::Codepage *found = Encoding::Find(text);
      return found == nullptr ? Encoding::Standard() : *found;
    }

    auto Foreign(std::string_view text, Version release) -> bool
    {
      return release < Version::R2007 && !text.empty() && Encoding::Find(text) == nullptr;
    }

    auto Deciding(const File::Variable &one, const Tags::Tag &held) -> bool
    {
      return (one.Name == Acadver && held.Code == Primary)
          || (one.Name == Codepage && held.Code == Secondary);
    }

    auto Worded(Fields::Writing &to, const Group &held) -> std::expected<void, Failure>
    {
      const auto *text = std::get_if<std::string>(&held.Value);
      return held.Code == Secondary && text != nullptr ? Fields::Word(to, held.Code, *text)
                                                       : Fields::Put(to, held);
    }

    auto Given(Fields::Writing &to, const Variable &one) -> std::expected<void, Failure>
    {
      if (one.Name == Acadver)
      {
        return Fields::Put(
            to, Group{.Code = Primary, .Value = std::string{DxfIO::Acadver(to.Release)}});
      }
      if (one.Name == Codepage && to.Text.Page != nullptr)
      {
        return Fields::Put(to, Group{.Code = Secondary, .Value = std::string{to.Text.Page->Name}});
      }
      if (one.Name == Codepage)
      {
        return Chained(one.Values, [&to](const Group &held) -> std::expected<void, Failure>
                       { return Worded(to, held); });
      }
      return Fields::Put(to, one.Values);
    }

    auto Written(Fields::Writing &to, const Variable &one) -> std::expected<void, Failure>
    {
      return Fields::Word(to, Name, one.Name)
          .and_then([&to, &one] -> std::expected<void, Failure> { return Given(to, one); });
    }

    auto Known(std::string_view wanted) -> bool
    {
      return std::ranges::contains(Rows::All(), wanted, &Row::Name);
    }

    auto Fits(const Variable &held, const Variable &wanted) -> bool
    {
      return held.Values.empty()
          || (held.Values.size() == wanted.Values.size()
              && held.Values.front().Code == wanted.Values.front().Code);
    }

    auto Rewritten(Fields::Writing &to, std::span<const Variable> all)
        -> std::expected<void, Failure>
    {
      const std::vector<Variable> fresh = Created(to.Release);
      const auto                  kept = [&](const Variable &wanted) -> std::expected<void, Failure>
      {
        const auto held = std::ranges::find(all, wanted.Name, &Variable::Name);
        return Written(to, held != all.end() && Fits(*held, wanted) ? *held : wanted);
      };
      const auto others = [&](const Variable &one) -> std::expected<void, Failure>
      {
        return Known(one.Name) ? std::expected<void, Failure>{} : Written(to, one);
      };
      return Chained(std::span{fresh}, kept)
          .and_then([&] -> std::expected<void, Failure> { return Chained(all, others); });
    }

    auto Missing(Fields::Writing &to, std::span<const Variable> all, std::string_view wanted)
        -> std::expected<void, Failure>
    {
      const bool page = to.Text.Page != nullptr && to.Release >= Version::R13;
      if (std::ranges::contains(all, wanted, &Variable::Name) || (wanted != Acadver && !page))
      {
        return {};
      }
      return Written(to, Variable{.Name = std::string{wanted}, .Values = {}});
    }
  } // namespace

  auto Declared(std::span<const File::Variable> all) -> Version
  {
    return Release(Valued(all, Acadver, Primary)).value_or(Version::R12);
  }

  auto Named(std::span<const File::Variable> all) -> const Encoding::Codepage &
  {
    return Paged(Valued(all, Codepage, Secondary));
  }

  auto Named(std::span<const Variable> all) -> const Encoding::Codepage &
  {
    return Paged(Valued(all, Codepage, Secondary));
  }

  auto Declared(std::span<const Variable> all) -> Version
  {
    return Release(Valued(all, Acadver, Primary)).value_or(Version::R12);
  }

  auto Opaque(std::span<const File::Variable> all) -> bool
  {
    return Foreign(Valued(all, Codepage, Secondary), Declared(all));
  }

  auto Opaque(std::span<const Variable> all) -> bool
  {
    return Foreign(Valued(all, Codepage, Secondary), Declared(all));
  }

  auto Read(std::span<const File::Variable> all, const Fields::Reading &with)
      -> std::vector<Variable>
  {
    std::vector<Variable> out;
    out.reserve(all.size());
    for (const File::Variable &one : all)
    {
      Variable kept{.Name = std::string{one.Name}, .Values = {}};
      for (const Tags::Tag &held : one.Values)
      {
        kept.Values.push_back(Deciding(one, held)
                                  ? Group{.Code = held.Code, .Value = std::string{Tags::Text(held)}}
                                  : Fields::Owned(held, with.Text));
      }
      out.push_back(std::move(kept));
    }
    return out;
  }

  auto Write(Fields::Writing &to, std::span<const Variable> all) -> std::expected<void, Failure>
  {
    if (to.Release != to.From)
    {
      return Rewritten(to, all);
    }
    std::expected<void, Failure> done = Missing(to, all, Acadver)
                                            .and_then([&to, all] -> std::expected<void, Failure>
                                                      { return Missing(to, all, Codepage); });
    for (const Variable &one : all)
    {
      done =
          done.and_then([&to, &one] -> std::expected<void, Failure> { return Written(to, one); });
    }
    return done;
  }
} // namespace DxfIO::Header
