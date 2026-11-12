#include "dxfio/raw.hpp"

#include "chained.hpp"
#include "fields/owned.hpp"
#include "fields/text.hpp"
#include "raw/io.hpp"

#include "dxfio/version.hpp"

#include <cstdint>

namespace DxfIO
{
  namespace
  {
    constexpr int Start = 0;

    constexpr int Flags = 90;

    constexpr int Instances = 91;

    auto Copied(const File::Record &one, const Fields::Reading &with) -> Raw
    {
      Raw                              out{.Type = std::string{one.Name}, .Groups = {}};
      const std::span<const Tags::Tag> body = one.Tags.subspan(1);
      out.Groups.reserve(body.size());
      for (const Tags::Tag &held : body)
      {
        out.Groups.push_back(Fields::Owned(held, with.Text));
      }
      return out;
    }

    auto Counted(Fields::Writing &to, const Group &one) -> std::expected<void, Failure>
    {
      if (one.Code == Instances && to.Release < Version::R2004)
      {
        return {};
      }
      const bool gained =
          one.Code == Flags && to.From < Version::R2004 && to.Release >= Version::R2004;
      return Fields::Put(to, one).and_then(
          [&to, gained] -> std::expected<void, Failure>
          {
            return gained ? Fields::Put(to,
                                        Group{
                                            .Code  = Instances,
                                            .Value = std::int64_t{0},
                                        })
                          : std::expected<void, Failure>{};
          });
    }

    auto Put(Fields::Writing &to, const Raw &one) -> std::expected<void, Failure>
    {
      return Fields::Word(to, Start, one.Type)
          .and_then(
              [&to, &one] -> std::expected<void, Failure>
              {
                return Chained(one.Groups, [&to](const Group &held) -> std::expected<void, Failure>
                               { return Counted(to, held); });
              });
    }
  } // namespace

  auto Copied(std::span<const File::Record> all, const Fields::Reading &with) -> std::vector<Raw>
  {
    std::vector<Raw> out;
    out.reserve(all.size());
    for (const File::Record &one : all)
    {
      out.push_back(Copied(one, with));
    }
    return out;
  }

  auto Put(Fields::Writing &to, std::span<const Raw> all) -> std::expected<void, Failure>
  {
    return Chained(all,
                   [&](const Raw &one) -> std::expected<void, Failure> { return Put(to, one); });
  }
} // namespace DxfIO
