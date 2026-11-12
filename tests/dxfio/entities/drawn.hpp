#ifndef DXFIO_TESTS_DXF_ENTITIES_DRAWN_HPP
#define DXFIO_TESTS_DXF_ENTITIES_DRAWN_HPP

#include "../autocad.hpp"
#include "../disk.hpp"
#include "../releases.hpp"

#include <string>
#include <string_view>

namespace DxfIO::Entities
{
  inline auto Drawn(std::string_view self, Version release = Version::R2018,
                    std::string_view stem = "entities") -> std::string
  {
    std::string text;
    for (const char one : Acad(Fixture(stem, release)))
    {
      if (one == '\n' && !text.ends_with('\r'))
      {
        text.push_back('\r');
      }
      text.push_back(one);
    }
    return Autocad(text, self);
  }
} // namespace DxfIO::Entities

#endif /* DXFIO_TESTS_DXF_ENTITIES_DRAWN_HPP */
