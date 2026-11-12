#include "../family.hpp"
#include "objects/io.hpp"
#include "shapes/data.hpp"
#include "shapes/defaulted.hpp"
#include "shapes/dictionary.hpp"
#include "shapes/gathered.hpp"
#include "shapes/layout.hpp"
#include "shapes/look.hpp"
#include "shapes/material.hpp"
#include "shapes/mleader/style.hpp"
#include "shapes/mline/style.hpp"
#include "shapes/placeholder.hpp"
#include "shapes/scale.hpp"
#include "shapes/settings.hpp"
#include "shapes/sortents/table.hpp"
#include "shapes/table/style.hpp"
#include "shapes/underlay/definition.hpp"
#include "shapes/wipeout/variables.hpp"

#include <string_view>
#include <variant>

namespace DxfIO::Objects
{
  auto Read(std::span<const File::Record> all, const Fields::Reading &with) -> std::vector<Object>
  {
    return Family::Read<Object>(all, with);
  }

  auto Type(const Object &one) -> std::string_view
  {
    return Family::Type(one);
  }

  auto Based(const Object &one) -> const DxfIO::Base &
  {
    return std::visit(
        [](const auto &kind) -> const DxfIO::Base &
        {
          using DxfIO::Based;
          return Based(kind);
        },
        one);
  }

  auto Write(Fields::Writing &to, std::span<const Object> all) -> std::expected<void, Failure>
  {
    return Family::Write<Object>(to, all);
  }
} // namespace DxfIO::Objects
