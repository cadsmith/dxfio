#ifndef DXFIO_OBJECTS_SHAPES_DEFAULTED_HPP
#define DXFIO_OBJECTS_SHAPES_DEFAULTED_HPP

#include "dictionary.hpp"
#include "fields/part.hpp"

#include "dxfio/objects/defaulted.hpp"

namespace DxfIO
{
  namespace Objects
  {
    inline auto Based(DictionaryWithDefault &one) -> DxfIO::Base &
    {
      return one.Dictionary.Base;
    }

    inline auto Based(const DictionaryWithDefault &one) -> const DxfIO::Base &
    {
      return one.Dictionary.Base;
    }
  } // namespace Objects

  template <>
  inline constexpr auto Shapes<Objects::DictionaryWithDefault> = std::tuple{
      Fields::Part{
          .Member = &Objects::DictionaryWithDefault::Dictionary,
          .Fields = Shapes<Objects::Dictionary>,
      },
      Fields::Subclass{.Marker = "AcDbDictionaryWithDefault"},
      Fields::Field{.Code = 340, .Member = &Objects::DictionaryWithDefault::Fallback},
  };

  template <>
  inline constexpr std::string_view Type<Objects::DictionaryWithDefault> = "ACDBDICTIONARYWDFLT";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_DEFAULTED_HPP */
