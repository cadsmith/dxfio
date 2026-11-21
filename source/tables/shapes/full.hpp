#ifndef DXFIO_TABLES_SHAPES_FULL_HPP
#define DXFIO_TABLES_SHAPES_FULL_HPP

#include "fields/field.hpp"
#include "fields/handles.hpp"
#include "fields/numbers.hpp"
#include "fields/part.hpp"
#include "fields/since.hpp"
#include "fields/text.hpp"

#include "dxfio/tables/dimstyle.hpp"

#include <tuple>

namespace DxfIO::Tables
{
  inline constexpr auto Names = std::tuple{
      Fields::Field{.Code = 5, .Member = &Arrows::Dimblk},
      Fields::Field{.Code = 6, .Member = &Arrows::Dimblk1},
      Fields::Field{.Code = 7, .Member = &Arrows::Dimblk2},
  };

  inline constexpr auto Added = std::tuple{
      Fields::Field{.Code = 179, .Member = &DimStyle::Dimadec},
      Fields::Field{.Code = 271, .Member = &DimStyle::Dimdec},
      Fields::Field{.Code = 272, .Member = &DimStyle::Dimtdec},
      Fields::Field{.Code = 273, .Member = &DimStyle::Dimaltu},
      Fields::Field{.Code = 274, .Member = &DimStyle::Dimalttd},
      Fields::Field{.Code = 275, .Member = &DimStyle::Dimaunit},
      Fields::Field{.Code = 276, .Member = &DimStyle::Dimfrac},
      Fields::Field{.Code = 277, .Member = &DimStyle::Dimlunit},
      Fields::Field{.Code = 278, .Member = &DimStyle::Dimdsep},
      Fields::Field{.Code = 280, .Member = &DimStyle::Dimjust},
      Fields::Field{.Code = 281, .Member = &DimStyle::Dimsd1},
      Fields::Field{.Code = 282, .Member = &DimStyle::Dimsd2},
      Fields::Field{.Code = 283, .Member = &DimStyle::Dimtolj},
      Fields::Field{.Code = 284, .Member = &DimStyle::Dimtzin},
      Fields::Field{.Code = 285, .Member = &DimStyle::Dimaltz},
      Fields::Field{.Code = 286, .Member = &DimStyle::Dimalttz},
      Fields::Field{.Code = 288, .Member = &DimStyle::Dimupt},
      Fields::Field{.Code = 340, .Member = &DimStyle::Dimtxsty},
      Fields::Field{.Code = 341, .Member = &DimStyle::Dimldrblk, .Fallback = Handle{}},
      Fields::Field{.Code = 342, .Member = &DimStyle::Dimblk, .Fallback = Handle{}},
      Fields::Field{.Code = 343, .Member = &DimStyle::Dimblk1, .Fallback = Handle{}},
      Fields::Field{.Code = 344, .Member = &DimStyle::Dimblk2, .Fallback = Handle{}},
  };

  inline constexpr auto Full = std::tuple{
      Fields::Field{.Code = 3, .Member = &DimStyle::Dimpost},
      Fields::Field{.Code = 4, .Member = &DimStyle::Dimapost},
      Fields::Part{.Member = &DimStyle::Arrows, .Fields = Names},
      Fields::Field{.Code = 40, .Member = &DimStyle::Dimscale},
      Fields::Field{.Code = 41, .Member = &DimStyle::Dimasz},
      Fields::Field{.Code = 42, .Member = &DimStyle::Dimexo},
      Fields::Field{.Code = 43, .Member = &DimStyle::Dimdli},
      Fields::Field{.Code = 44, .Member = &DimStyle::Dimexe},
      Fields::Field{.Code = 45, .Member = &DimStyle::Dimrnd},
      Fields::Field{.Code = 46, .Member = &DimStyle::Dimdle},
      Fields::Field{.Code = 47, .Member = &DimStyle::Dimtp},
      Fields::Field{.Code = 48, .Member = &DimStyle::Dimtm},
      Fields::Field{.Code = 140, .Member = &DimStyle::Dimtxt},
      Fields::Field{.Code = 141, .Member = &DimStyle::Dimcen},
      Fields::Field{.Code = 142, .Member = &DimStyle::Dimtsz},
      Fields::Field{.Code = 143, .Member = &DimStyle::Dimaltf},
      Fields::Field{.Code = 144, .Member = &DimStyle::Dimlfac},
      Fields::Field{.Code = 145, .Member = &DimStyle::Dimtvp},
      Fields::Field{.Code = 146, .Member = &DimStyle::Dimtfac},
      Fields::Field{.Code = 147, .Member = &DimStyle::Dimgap},
      Fields::Field{.Code = 71, .Member = &DimStyle::Dimtol},
      Fields::Field{.Code = 72, .Member = &DimStyle::Dimlim},
      Fields::Field{.Code = 73, .Member = &DimStyle::Dimtih},
      Fields::Field{.Code = 74, .Member = &DimStyle::Dimtoh},
      Fields::Field{.Code = 75, .Member = &DimStyle::Dimse1},
      Fields::Field{.Code = 76, .Member = &DimStyle::Dimse2},
      Fields::Field{.Code = 77, .Member = &DimStyle::Dimtad},
      Fields::Field{.Code = 78, .Member = &DimStyle::Dimzin},
      Fields::Field{.Code = 170, .Member = &DimStyle::Dimalt},
      Fields::Field{.Code = 171, .Member = &DimStyle::Dimaltd},
      Fields::Field{.Code = 172, .Member = &DimStyle::Dimtofl},
      Fields::Field{.Code = 173, .Member = &DimStyle::Dimsah},
      Fields::Field{.Code = 174, .Member = &DimStyle::Dimtix},
      Fields::Field{.Code = 175, .Member = &DimStyle::Dimsoxd},
      Fields::Field{.Code = 176, .Member = &DimStyle::Dimclrd},
      Fields::Field{.Code = 177, .Member = &DimStyle::Dimclre},
      Fields::Field{.Code = 178, .Member = &DimStyle::Dimclrt},
      Fields::Since{.Release = Version::R13, .Fields = Added},
  };
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_SHAPES_FULL_HPP */
