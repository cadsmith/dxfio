#ifndef DXFIO_ENTITIES_SHAPES_ATTRIBUTE_HPP
#define DXFIO_ENTITIES_SHAPES_ATTRIBUTE_HPP

#include "fields/custom.hpp"
#include "fields/field.hpp"
#include "fields/first.hpp"
#include "fields/numbers.hpp"
#include "fields/since.hpp"
#include "fields/text.hpp"

#include "dxfio/version.hpp"

#include <expected>
#include <optional>
#include <tuple>

namespace DxfIO::Entities
{
  inline constexpr int Latch = 280;

  template <class KindT>
  void Locked(KindT &one, Fields::Walk &in)
  {
    const std::optional<bool> last = Fields::Value<bool>::Read(in, Latch);
    one.Locked                     = last.value_or(one.Revision != 0);
    one.Revision                   = last ? one.Revision : 0;
  }

  template <class KindT>
  auto Locking(const KindT &one, Fields::Writing &to) -> std::expected<void, Failure>
  {
    return Fields::Value<bool>::Write(to, Latch, one.Locked);
  }

  template <class KindT>
  inline constexpr auto Revised = Fields::Since{
      .Release = Version::R2010,
      .Fields =
          std::tuple{
              Fields::First{
                  .Code = Latch,
                  .Fields =
                      std::tuple{
                          Fields::Field{
                              .Code   = Latch,
                              .Member = &KindT::Revision,
                          },
                      },
              },
          },
  };

  template <class KindT>
  inline constexpr auto Labeled = std::tuple{
      Fields::Field{.Code = 2, .Member = &KindT::Tag},
      Fields::Field{.Code = 70, .Member = &KindT::Flags},
      Fields::Field{.Code = 73, .Member = &KindT::Length, .Fallback = 0},
      Fields::Field{.Code = 74, .Member = &KindT::Vertical, .Fallback = 0},
      Fields::Since{
          .Release = Version::R2007,
          .Fields =
              std::tuple{
                  Fields::Custom<KindT>{
                      .Read  = Locked<KindT>,
                      .Write = Locking<KindT>,
                  },
              },
      },
  };
} // namespace DxfIO::Entities

#endif /* DXFIO_ENTITIES_SHAPES_ATTRIBUTE_HPP */
