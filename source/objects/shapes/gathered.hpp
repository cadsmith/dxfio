#ifndef DXFIO_OBJECTS_SHAPES_GATHERED_HPP
#define DXFIO_OBJECTS_SHAPES_GATHERED_HPP

#include "fields/context.hpp"
#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/flat.hpp"
#include "fields/handles.hpp"
#include "fields/many.hpp"
#include "fields/numbers.hpp"
#include "fields/owned.hpp"
#include "fields/subclass.hpp"
#include "fields/text.hpp"
#include "fields/walk.hpp"
#include "shapes.hpp"

#include "dxfio/failure.hpp"
#include "dxfio/group.hpp"
#include "dxfio/objects/dictionaryvar.hpp"
#include "dxfio/objects/group.hpp"
#include "dxfio/objects/image/definition.hpp"
#include "dxfio/objects/xrecord.hpp"
#include "dxfio/version.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace DxfIO
{
  namespace Objects
  {
    inline constexpr int Flag = 280;

    inline auto Flagged(const std::vector<DxfIO::Group> &data, Version release)
        -> std::optional<int>
    {
      if (release < Version::R2000 || data.empty() || data.front().Code != Flag)
      {
        return std::nullopt;
      }
      const auto *value = std::get_if<std::int64_t>(&data.front().Value);
      if (value == nullptr)
      {
        return std::nullopt;
      }
      return static_cast<int>(*value);
    }

    inline void Held(XRecord &one, Fields::Walk &in)
    {
      std::vector<DxfIO::Group> data = in.Rest();
      in.Spend();
      if (const std::optional<int> cloning = Flagged(data, in.With().Release))
      {
        one.Cloning = *cloning;
        data.erase(data.begin());
      }
      one.Data = std::move(data);
    }

    inline auto Holding(const XRecord &one, Fields::Writing &to) -> std::expected<void, Failure>
    {
      const std::expected<void, Failure> flagged =
          to.Release >= Version::R2000 ? Fields::Value<int>::Write(to, Flag, one.Cloning)
                                       : std::expected<void, Failure>{};
      return flagged.and_then([&to, &one] -> std::expected<void, Failure>
                              { return Fields::Put(to, one.Data); });
    }
  } // namespace Objects

  template <>
  inline constexpr auto Shapes<Objects::DictionaryVar> = std::tuple{
      Fields::Subclass{.Marker = "DictionaryVariables"},
      Fields::Field{.Code = 280, .Member = &Objects::DictionaryVar::Schema},
      Fields::Field{.Code = 1, .Member = &Objects::DictionaryVar::Value},
  };

  template <>
  inline constexpr auto Shapes<Objects::XRecord> = std::tuple{
      Fields::Subclass{.Marker = "AcDbXrecord"},
      Fields::Custom<Objects::XRecord>{
          .Read  = Objects::Held,
          .Write = Objects::Holding,
      },
  };

  template <>
  inline constexpr auto Shapes<Objects::Group> = std::tuple{
      Fields::Subclass{.Marker = "AcDbGroup"},
      Fields::Field{.Code = 300, .Member = &Objects::Group::Description},
      Fields::Field{.Code = 70, .Member = &Objects::Group::Unnamed},
      Fields::Field{.Code = 71, .Member = &Objects::Group::Selectable},
      Fields::Many{.Code = 340, .Member = &Objects::Group::Members},
  };

  template <>
  inline constexpr auto Shapes<Objects::ImageDef> = std::tuple{
      Fields::Subclass{.Marker = "AcDbRasterImageDef"},
      Fields::Field{.Code = 90, .Member = &Objects::ImageDef::Version},
      Fields::Field{.Code = 1, .Member = &Objects::ImageDef::File},
      Fields::Flat{.Code = 10, .Member = &Objects::ImageDef::Size},
      Fields::Flat{.Code = 11, .Member = &Objects::ImageDef::Pixel},
      Fields::Field{.Code = 280, .Member = &Objects::ImageDef::Loaded},
      Fields::Field{.Code = 281, .Member = &Objects::ImageDef::Units},
  };

  template <>
  inline constexpr auto Shapes<Objects::ImageDefReactor> = std::tuple{
      Fields::Subclass{.Marker = "AcDbRasterImageDefReactor"},
      Fields::Field{.Code = 90, .Member = &Objects::ImageDefReactor::Version},
      Fields::Field{.Code = 330, .Member = &Objects::ImageDefReactor::Image},
  };

  template <>
  inline constexpr auto Shapes<Objects::RasterVariables> = std::tuple{
      Fields::Subclass{.Marker = "AcDbRasterVariables"},
      Fields::Field{.Code = 90, .Member = &Objects::RasterVariables::Version},
      Fields::Field{.Code = 70, .Member = &Objects::RasterVariables::Frame},
      Fields::Field{.Code = 71, .Member = &Objects::RasterVariables::Quality},
      Fields::Field{.Code = 72, .Member = &Objects::RasterVariables::Units},
  };

  template <>
  inline constexpr std::string_view Type<Objects::DictionaryVar> = "DICTIONARYVAR";

  template <>
  inline constexpr std::string_view Type<Objects::XRecord> = "XRECORD";

  template <>
  inline constexpr std::string_view Type<Objects::Group> = "GROUP";

  template <>
  inline constexpr std::string_view Type<Objects::ImageDef> = "IMAGEDEF";

  template <>
  inline constexpr std::string_view Type<Objects::ImageDefReactor> = "IMAGEDEF_REACTOR";

  template <>
  inline constexpr std::string_view Type<Objects::RasterVariables> = "RASTERVARIABLES";
} // namespace DxfIO

#endif /* DXFIO_OBJECTS_SHAPES_GATHERED_HPP */
