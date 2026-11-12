#ifndef DXFIO_FIELDS_BRACED_HPP
#define DXFIO_FIELDS_BRACED_HPP

#include "chained.hpp"
#include "fields/read.hpp"
#include "fields/text.hpp"

#include <expected>
#include <string_view>
#include <utility>
#include <vector>

namespace DxfIO::Fields
{
  inline constexpr std::string_view Closed = "}";

  struct Literal
  {
    int              Code{};
    std::string_view Text;
  };

  template <class OwnerT>
  void Take(const Literal &shape, OwnerT & /*unused*/, Walk &in)
  {
    static_cast<void>(in.Take(shape.Code));
  }

  template <class OwnerT>
  auto Put(const Literal &shape, const OwnerT & /*unused*/, Writing &to)
      -> std::expected<void, Failure>
  {
    return Word(to, shape.Code, shape.Text);
  }

  template <class OwnerT, class ElementT, class FieldsT>
  struct Braced
  {
    int                   Opener{};
    std::string_view      Name;
    int                   Closer{};
    std::vector<ElementT> OwnerT::*Member{};
    FieldsT                        Fields{};
  };

  template <class OwnerT, class ElementT, class FieldsT>
  void Take(const Braced<OwnerT, ElementT, FieldsT> &shape, OwnerT &owner, Walk &in)
  {
    while (in.Take(shape.Opener) != nullptr)
    {
      ElementT one{};
      Read(one, in, shape.Fields);
      if (in.Take(shape.Closer) == nullptr)
      {
        in.Refuse();
      }
      (owner.*shape.Member).push_back(std::move(one));
    }
  }

  template <class OwnerT, class ElementT, class FieldsT>
  auto Put(const Braced<OwnerT, ElementT, FieldsT> &shape, const OwnerT &owner, Writing &to)
      -> std::expected<void, Failure>
  {
    return Chained(owner.*shape.Member,
                   [&](const ElementT &element) -> auto
                   {
                     return Word(to, shape.Opener, shape.Name)
                         .and_then([&] -> auto { return Write(element, to, shape.Fields); })
                         .and_then([&] -> auto { return Word(to, shape.Closer, Closed); });
                   });
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_BRACED_HPP */
