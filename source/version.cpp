#include "dxfio/version.hpp"

#include "ascii.hpp"
#include "tags/blanks.hpp"
#include "tags/whole.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <utility>

namespace DxfIO
{
  namespace
  {
    struct Step
    {
      Version          Release{};
      std::string_view Acadver;
      std::string_view Name;
    };

    constexpr std::array Ladder{
        Step{.Release = Version::R10  , .Acadver = "AC1006", .Name = "R10"  },
        Step{.Release = Version::R12  , .Acadver = "AC1009", .Name = "R12"  },
        Step{.Release = Version::R13  , .Acadver = "AC1012", .Name = "R13"  },
        Step{.Release = Version::R14  , .Acadver = "AC1014", .Name = "R14"  },
        Step{.Release = Version::R2000, .Acadver = "AC1015", .Name = "R2000"},
        Step{.Release = Version::R2004, .Acadver = "AC1018", .Name = "R2004"},
        Step{.Release = Version::R2007, .Acadver = "AC1021", .Name = "R2007"},
        Step{.Release = Version::R2010, .Acadver = "AC1024", .Name = "R2010"},
        Step{.Release = Version::R2013, .Acadver = "AC1027", .Name = "R2013"},
        Step{.Release = Version::R2018, .Acadver = "AC1032", .Name = "R2018"},
    };

    auto At(Version release) -> const Step &
    {
      return Ladder.at(std::to_underlying(release));
    }

    constexpr std::string_view Prefix = "AC";

    constexpr std::size_t Digits = 4;

    auto Prefixed(std::string_view word) -> bool
    {
      return DxfIO::Ascii::Same(word.substr(0, Prefix.size()), Prefix);
    }

    auto Number(std::string_view code) -> std::optional<int>
    {
      const std::string_view word = Tags::Trimmed(code);
      if (word.size() != Prefix.size() + Digits || !Prefixed(word))
      {
        return std::nullopt;
      }
      return Tags::Whole(word.substr(Prefix.size()));
    }
  } // namespace

  auto Acadver(Version release) -> std::string_view
  {
    return At(release).Acadver;
  }

  auto Name(Version release) -> std::string_view
  {
    return At(release).Name;
  }

  auto Release(std::string_view code) -> std::optional<Version>
  {
    const std::optional<int> wanted = Number(code);
    if (!wanted)
    {
      return std::nullopt;
    }
    Version found = Version::R10;
    for (const Step &one : Ladder)
    {
      if (Number(one.Acadver) <= wanted)
      {
        found = one.Release;
      }
    }
    return found;
  }
} // namespace DxfIO
