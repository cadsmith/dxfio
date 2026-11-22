#ifndef DXFIO_OBJECTS_SHAPES_DICTIONARY_HPP
#define DXFIO_OBJECTS_SHAPES_DICTIONARY_HPP

#include "fields/context.hpp"
#include "fields/custom.hpp"
#include "fields/every.hpp"
#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/optional.hpp"
#include "fields/since.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "fields/walk.hpp"
#include "shapes.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/objects/dictionary.hpp"

#include <expected>
#include <string_view>
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

    inline constexpr std::string_view WithDefault = "ACDBDICTIONARYWDFLT";

    inline constexpr Fields::Subclass Defaulting{.Marker = "AcDbDictionaryWithDefault"};

    inline constexpr Fields::Field DefaultPointer{.Code = 340, .Member = &Dictionary::Default};

    /**
     *  Reads the default of an ACDBDICTIONARYWDFLT, null when its 340 is missing.
     */
    inline void Defaulted(Dictionary &one, Fields::Walk &in)
    {
      if (in.Subclass(Defaulting.Marker))
      {
        one.Default = Handle{};
        Fields::Take(DefaultPointer, one, in);
      }
    }

    /**
     *  A dictionary read as an ACDBDICTIONARYWDFLT stays one, its default null, though its
     *  AcDbDictionaryWithDefault part is missing, so it is written back under the type it came in.
     */
    inline void Aliased(Dictionary &one, std::string_view type)
    {
      if (type == WithDefault && !one.Default)
      {
        one.Default = Handle{};
      }
    }

    inline auto Defaults(const Dictionary &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      if (!one.Default)
      {
        return {};
      }
      return Fields::Mark(Defaulting, to)
          .and_then([&one, &to] -> auto { return Fields::Put(DefaultPointer, one, to); });
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
                      .Member   = &Objects::Dictionary::HardOwner,
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
      Fields::Custom<Objects::Dictionary>{
          .Read  = Objects::Defaulted,
          .Write = Objects::Defaults,
      },
  };

  template <>
  inline constexpr std::string_view Type<Objects::Dictionary> = "DICTIONARY";

  namespace Objects
  {
    /**
     *  The type a dictionary is written as: an ACDBDICTIONARYWDFLT when it has a default.
     */
    inline auto RecordType(const Dictionary &one) -> std::string_view
    {
      return one.Default ? WithDefault : DxfIO::Type<Dictionary>;
    }
  } // namespace Objects
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_DICTIONARY_HPP */
