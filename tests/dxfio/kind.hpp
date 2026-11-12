#ifndef DXFIO_TESTS_DXF_KIND_HPP
#define DXFIO_TESTS_DXF_KIND_HPP

#include "fields/fixture.hpp"
#include "file/cursor.hpp"
#include "filing.hpp"
#include "tags/tags.hpp"

#include <doctest/doctest.h>

#include <string>
#include <string_view>
#include <vector>

namespace DxfIO
{
  template <class KindT>
  auto Read(std::string_view text, Version release = Version::R2018) -> KindT
  {
    const auto all = Tags::All(text);
    REQUIRE(all);
    File::Cursor in{*all};
    const auto   one = in.Take();
    REQUIRE(one);
    std::vector<Issue>    issues;
    const Fields::Reading with{.Release = release, .Text = Encoding::Utf8(), .Issues = &issues};
    KindT                 out = Taken<KindT>(*one, with);
    CHECK(issues.empty());
    return out;
  }

  template <class KindT>
  auto Written(const KindT &one, Version release = Version::R2018) -> std::string
  {
    Fields::Sheet page{release};
    REQUIRE(Given(page.To, one));
    return page.Out;
  }

  template <class KindT>
  auto Replayed(const std::string &text, Version release = Version::R2018) -> KindT
  {
    KindT out = Read<KindT>(text, release);
    CHECK(out.Record.Base.Extra.empty());
    CHECK(Written(out, release) == text);
    return out;
  }
} // namespace DxfIO

#endif /* DXFIO_TESTS_DXF_KIND_HPP */
