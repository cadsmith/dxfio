#include "dxfio/color.hpp"

#include "dxfio/colors.hpp"

#include <cstdint>
#include <optional>
#include <utility>

namespace DxfIO
{
  namespace
  {
    constexpr std::uint32_t RgbBits = 0xFFFFFF;

    constexpr unsigned MethodShift = 24;

    constexpr std::uint32_t AlphaBits = 0xFF;
  } // namespace

  auto MethodOf(const Entities::Record &record) -> ColorMethod
  {
    if (record.Rgb)
    {
      return ColorMethod::Rgb;
    }
    if (record.Color == Colors::ByBlock)
    {
      return ColorMethod::ByBlock;
    }
    return record.Color == Colors::ByLayer ? ColorMethod::ByLayer : ColorMethod::Index;
  }

  auto RgbOf(const Entities::Record &record) -> std::optional<std::uint32_t>
  {
    if (!record.Rgb)
    {
      return std::nullopt;
    }
    return static_cast<std::uint32_t>(*record.Rgb) & RgbBits;
  }

  auto TransparencyMethodOf(const Entities::Record &record) -> std::optional<TransparencyMethod>
  {
    if (!record.Transparency)
    {
      return std::nullopt;
    }
    const auto method = static_cast<std::uint32_t>(*record.Transparency) >> MethodShift;
    switch (method)
    {
    case std::to_underlying(TransparencyMethod::ByLayer):
      return TransparencyMethod::ByLayer;
    case std::to_underlying(TransparencyMethod::ByBlock):
      return TransparencyMethod::ByBlock;
    case std::to_underlying(TransparencyMethod::Alpha):
      return TransparencyMethod::Alpha;
    default:
      return std::nullopt;
    }
  }

  auto AlphaOf(const Entities::Record &record) -> std::optional<std::uint8_t>
  {
    if (!record.Transparency || TransparencyMethodOf(record) != TransparencyMethod::Alpha)
    {
      return std::nullopt;
    }
    return static_cast<std::uint8_t>(static_cast<std::uint32_t>(*record.Transparency) & AlphaBits);
  }
} // namespace DxfIO
