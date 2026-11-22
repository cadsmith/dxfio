#ifndef DXFIO_TABLES_SHAPES_DIMSTYLE_HPP
#define DXFIO_TABLES_SHAPES_DIMSTYLE_HPP

#include "../../filing.hpp"
#include "fields/first.hpp"
#include "fields/until.hpp"
#include "full.hpp"
#include "record.hpp"

#include <string_view>

namespace DxfIO
{
  namespace Tables
  {
    inline constexpr auto Sparse = std::tuple{
        Fields::Field{
            .Code     = 3,
            .Member   = &DimStyle::Dimpost,
            .Fallback = std::string_view{},
        },
        Fields::Field{
            .Code     = 4,
            .Member   = &DimStyle::Dimapost,
            .Fallback = std::string_view{},
        },
        Fields::Field{.Code = 40, .Member = &DimStyle::Dimscale, .Fallback = 1.0},
        Fields::Field{.Code = 41, .Member = &DimStyle::Dimasz, .Fallback = 0.18},
        Fields::Field{.Code = 42, .Member = &DimStyle::Dimexo, .Fallback = 0.0625},
        Fields::Field{
            .Code     = 43,
            .Member   = &DimStyle::Dimdli,
            .Fallback = 0.38,
        },
        Fields::Field{.Code = 44, .Member = &DimStyle::Dimexe, .Fallback = 0.18},
        Fields::Field{.Code = 45, .Member = &DimStyle::Dimrnd, .Fallback = 0.0},
        Fields::Field{.Code = 46, .Member = &DimStyle::Dimdle, .Fallback = 0.0},
        Fields::Field{.Code = 47, .Member = &DimStyle::Dimtp, .Fallback = 0.0},
        Fields::Field{.Code = 48, .Member = &DimStyle::Dimtm, .Fallback = 0.0},
        Fields::Field{
            .Code     = 49,
            .Member   = &DimStyle::Dimfxl,
            .Fallback = 1.0,
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 50,
            .Member   = &DimStyle::Dimjogang,
            .Fallback = 45,
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 69,
            .Member   = &DimStyle::Dimtfill,
            .Fallback = 0,
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 70,
            .Member   = &DimStyle::Dimtfillclr,
            .Fallback = 0,
            .Since    = Version::R2007,
        },
        Fields::Field{.Code = 71, .Member = &DimStyle::Dimtol, .Fallback = false},
        Fields::Field{.Code = 72, .Member = &DimStyle::Dimlim, .Fallback = false},
        Fields::Field{.Code = 73, .Member = &DimStyle::Dimtih, .Fallback = true},
        Fields::Field{.Code = 74, .Member = &DimStyle::Dimtoh, .Fallback = true},
        Fields::Field{.Code = 75, .Member = &DimStyle::Dimse1, .Fallback = false},
        Fields::Field{.Code = 76, .Member = &DimStyle::Dimse2, .Fallback = false},
        Fields::Field{.Code = 77, .Member = &DimStyle::Dimtad, .Fallback = 0},
        Fields::Field{.Code = 78, .Member = &DimStyle::Dimzin, .Fallback = 0},
        Fields::Field{.Code = 79, .Member = &DimStyle::Dimazin, .Fallback = 0},
        Fields::Field{
            .Code     = 90,
            .Member   = &DimStyle::Dimarcsym,
            .Fallback = 0,
            .Since    = Version::R2007,
        },
        Fields::Field{.Code = 140, .Member = &DimStyle::Dimtxt, .Fallback = 0.18},
        Fields::Field{.Code = 141, .Member = &DimStyle::Dimcen, .Fallback = 0.09},
        Fields::Field{.Code = 142, .Member = &DimStyle::Dimtsz, .Fallback = 0.0},
        Fields::Field{.Code = 143, .Member = &DimStyle::Dimaltf, .Fallback = 25.4},
        Fields::Field{.Code = 144, .Member = &DimStyle::Dimlfac, .Fallback = 1.0},
        Fields::Field{.Code = 145, .Member = &DimStyle::Dimtvp, .Fallback = 0.0},
        Fields::Field{.Code = 146, .Member = &DimStyle::Dimtfac, .Fallback = 1.0},
        Fields::Field{.Code = 147, .Member = &DimStyle::Dimgap, .Fallback = 0.09},
        Fields::Field{.Code = 148, .Member = &DimStyle::Dimaltrnd, .Fallback = 0.0},
        Fields::Field{.Code = 170, .Member = &DimStyle::Dimalt, .Fallback = false},
        Fields::Field{.Code = 171, .Member = &DimStyle::Dimaltd, .Fallback = 2},
        Fields::Field{.Code = 172, .Member = &DimStyle::Dimtofl, .Fallback = false},
        Fields::Field{.Code = 173, .Member = &DimStyle::Dimsah, .Fallback = false},
        Fields::Field{.Code = 174, .Member = &DimStyle::Dimtix, .Fallback = false},
        Fields::Field{.Code = 175, .Member = &DimStyle::Dimsoxd, .Fallback = false},
        Fields::Field{.Code = 176, .Member = &DimStyle::Dimclrd, .Fallback = 0},
        Fields::Field{.Code = 177, .Member = &DimStyle::Dimclre, .Fallback = 0},
        Fields::Field{.Code = 178, .Member = &DimStyle::Dimclrt, .Fallback = 0},
        Fields::Field{.Code = 179, .Member = &DimStyle::Dimadec, .Fallback = 0},
        Fields::Field{.Code = 271, .Member = &DimStyle::Dimdec, .Fallback = 4},
        Fields::Field{.Code = 272, .Member = &DimStyle::Dimtdec, .Fallback = 4},
        Fields::Field{.Code = 273, .Member = &DimStyle::Dimaltu, .Fallback = 2},
        Fields::Field{.Code = 274, .Member = &DimStyle::Dimalttd, .Fallback = 2},
        Fields::Field{.Code = 275, .Member = &DimStyle::Dimaunit, .Fallback = 0},
        Fields::Field{.Code = 276, .Member = &DimStyle::Dimfrac, .Fallback = 0},
        Fields::Field{.Code = 277, .Member = &DimStyle::Dimlunit, .Fallback = 2},
        Fields::Field{
            .Code     = 278,
            .Member   = &DimStyle::Dimdsep,
            .Fallback = int{'.'},
        },
        Fields::Field{.Code = 279, .Member = &DimStyle::Dimtmove, .Fallback = 0},
        Fields::Field{.Code = 280, .Member = &DimStyle::Dimjust, .Fallback = 0},
        Fields::Field{.Code = 281, .Member = &DimStyle::Dimsd1, .Fallback = false},
        Fields::Field{.Code = 282, .Member = &DimStyle::Dimsd2, .Fallback = false},
        Fields::Field{.Code = 283, .Member = &DimStyle::Dimtolj, .Fallback = 1},
        Fields::Field{.Code = 284, .Member = &DimStyle::Dimtzin, .Fallback = 0},
        Fields::Field{.Code = 285, .Member = &DimStyle::Dimaltz, .Fallback = 0},
        Fields::Field{.Code = 286, .Member = &DimStyle::Dimalttz, .Fallback = 0},
        Fields::Field{.Code = 288, .Member = &DimStyle::Dimupt, .Fallback = false},
        Fields::Field{.Code = 289, .Member = &DimStyle::Dimatfit, .Fallback = 3},
        Fields::Field{
            .Code     = 290,
            .Member   = &DimStyle::Dimfxlon,
            .Fallback = false,
            .Since    = Version::R2007,
        },
        Fields::Field{.Code = 340, .Member = &DimStyle::Dimtxsty, .Fallback = Handle{}},
        Fields::Field{.Code = 341, .Member = &DimStyle::Dimldrblk, .Fallback = Handle{}},
        Fields::Field{.Code = 342, .Member = &DimStyle::Dimblk, .Fallback = Handle{}},
        Fields::Field{.Code = 343, .Member = &DimStyle::Dimblk1, .Fallback = Handle{}},
        Fields::Field{.Code = 344, .Member = &DimStyle::Dimblk2, .Fallback = Handle{}},
        Fields::Field{
            .Code     = 345,
            .Member   = &DimStyle::Dimltype,
            .Fallback = Handle{},
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 346,
            .Member   = &DimStyle::Dimltex1,
            .Fallback = Handle{},
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 347,
            .Member   = &DimStyle::Dimltex2,
            .Fallback = Handle{},
            .Since    = Version::R2007,
        },
        Fields::Field{.Code = 371, .Member = &DimStyle::Dimlwd, .Fallback = -2},
        Fields::Field{.Code = 372, .Member = &DimStyle::Dimlwe, .Fallback = -2},
    };
  } // namespace Tables

  template <>
  inline constexpr int Self<Tables::DimStyle> = 105;

  template <>
  inline constexpr auto Shapes<Tables::DimStyle> = std::tuple{
      Tables::Opening,
      Fields::Subclass{.Marker = "AcDbDimStyleTableRecord"},
      Fields::Part{
          .Member = &Tables::DimStyle::Record,
          .Fields = std::tuple{Fields::Field{.Code = 2, .Member = &Tables::Record::Name},
                               Fields::First{
                                   .Code   = 70,
                                   .Fields = std::tuple{Fields::Field{
                                       .Code   = 70,
                                       .Member = &Tables::Record::Flags,
                                   }},
                               }},
      },
      Fields::Until{.Release = Version::R14, .Fields = Tables::Full},
      Fields::Since{.Release = Version::R2000, .Fields = Tables::Sparse},
  };

  template <>
  inline constexpr std::string_view Type<Tables::DimStyle> = "DIMSTYLE";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_DIMSTYLE_HPP */
