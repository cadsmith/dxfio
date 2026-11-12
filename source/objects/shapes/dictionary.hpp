#ifndef DXFIO_OBJECTS_SHAPES_DICTIONARY_HPP
#define DXFIO_OBJECTS_SHAPES_DICTIONARY_HPP

#include "fields/context.hpp"
#include "fields/custom.hpp"
#include "fields/every.hpp"
#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "fields/walk.hpp"
#include "shapes.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/objects/dictionary.hpp"

#include <expected>
#include <tuple>

namespace DxfIO
{
  namespace Objects
  {
    inline constexpr Fields::Field Pointer{.Code = 350, .Member = &Entry::Soft};

    inline void Pointed(Entry &one, Fields::Walk &in)
    {
      Fields::Take(Pointer, one, in);
    }

    inline auto Pointing(const Entry &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      return one.Soft || !one.Hard ? Fields::Put(Pointer, one, to) : std::expected<void, Failure>{};
    }
  } // namespace Objects

  template <>
  inline constexpr auto Shapes<Objects::Entry> = std::tuple{
      Fields::Field{.Code = 3, .Member = &Objects::Entry::Name},
      Fields::Custom<Objects::Entry>{
          .Read  = Objects::Pointed,
          .Write = Objects::Pointing,
      },
      Fields::Field{.Code = 360, .Member = &Objects::Entry::Hard, .Fallback = Handle{}},
  };

  template <>
  inline constexpr auto Shapes<Objects::Dictionary> = std::tuple{
      Fields::Subclass{.Marker = "AcDbDictionary"},
      Fields::Since{
          .Release = Version::R2000,
          .Fields =
              std::tuple{
                  Fields::Field{
                      .Code     = 280,
                      .Member   = &Objects::Dictionary::Hard,
                      .Fallback = false,
                  },
                  Fields::Field{
                      .Code   = 281,
                      .Member = &Objects::Dictionary::Cloning,
                  },
              },
      },
      Fields::Every{
          .Opener = 3,
          .Member = &Objects::Dictionary::Entries,
          .Fields = Shapes<Objects::Entry>,
      },
  };

  template <>
  inline constexpr std::string_view Type<Objects::Dictionary> = "DICTIONARY";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_DICTIONARY_HPP */
