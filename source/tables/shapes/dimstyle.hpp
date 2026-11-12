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
            .Member   = &Dimstyle::Dimpost,
            .Fallback = std::string_view{},
        },
        Fields::Field{
            .Code     = 4,
            .Member   = &Dimstyle::Dimapost,
            .Fallback = std::string_view{},
        },
        Fields::Field{.Code = 40, .Member = &Dimstyle::Dimscale, .Fallback = 1.0},
        Fields::Field{.Code = 41, .Member = &Dimstyle::Dimasz, .Fallback = Size},
        Fields::Field{.Code = 42, .Member = &Dimstyle::Dimexo, .Fallback = Offset},
        Fields::Field{
            .Code     = 43,
            .Member   = &Dimstyle::Dimdli,
            .Fallback = Increment,
        },
        Fields::Field{.Code = 44, .Member = &Dimstyle::Dimexe, .Fallback = Size},
        Fields::Field{.Code = 45, .Member = &Dimstyle::Dimrnd, .Fallback = 0.0},
        Fields::Field{.Code = 46, .Member = &Dimstyle::Dimdle, .Fallback = 0.0},
        Fields::Field{.Code = 47, .Member = &Dimstyle::Dimtp, .Fallback = 0.0},
        Fields::Field{.Code = 48, .Member = &Dimstyle::Dimtm, .Fallback = 0.0},
        Fields::Field{
            .Code     = 49,
            .Member   = &Dimstyle::Dimfxl,
            .Fallback = 1.0,
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 50,
            .Member   = &Dimstyle::Dimjogang,
            .Fallback = Jog,
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 69,
            .Member   = &Dimstyle::Dimtfill,
            .Fallback = 0,
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 70,
            .Member   = &Dimstyle::Dimtfillclr,
            .Fallback = 0,
            .Since    = Version::R2007,
        },
        Fields::Field{.Code = 71, .Member = &Dimstyle::Dimtol, .Fallback = 0},
        Fields::Field{.Code = 72, .Member = &Dimstyle::Dimlim, .Fallback = 0},
        Fields::Field{.Code = 73, .Member = &Dimstyle::Dimtih, .Fallback = 1},
        Fields::Field{.Code = 74, .Member = &Dimstyle::Dimtoh, .Fallback = 1},
        Fields::Field{.Code = 75, .Member = &Dimstyle::Dimse1, .Fallback = 0},
        Fields::Field{.Code = 76, .Member = &Dimstyle::Dimse2, .Fallback = 0},
        Fields::Field{.Code = 77, .Member = &Dimstyle::Dimtad, .Fallback = 0},
        Fields::Field{.Code = 78, .Member = &Dimstyle::Dimzin, .Fallback = 0},
        Fields::Field{.Code = 79, .Member = &Dimstyle::Dimazin, .Fallback = 0},
        Fields::Field{
            .Code     = 90,
            .Member   = &Dimstyle::Dimarcsym,
            .Fallback = 0,
            .Since    = Version::R2007,
        },
        Fields::Field{.Code = 140, .Member = &Dimstyle::Dimtxt, .Fallback = Size},
        Fields::Field{.Code = 141, .Member = &Dimstyle::Dimcen, .Fallback = Mark},
        Fields::Field{.Code = 142, .Member = &Dimstyle::Dimtsz, .Fallback = 0.0},
        Fields::Field{.Code = 143, .Member = &Dimstyle::Dimaltf, .Fallback = Inch},
        Fields::Field{.Code = 144, .Member = &Dimstyle::Dimlfac, .Fallback = 1.0},
        Fields::Field{.Code = 145, .Member = &Dimstyle::Dimtvp, .Fallback = 0.0},
        Fields::Field{.Code = 146, .Member = &Dimstyle::Dimtfac, .Fallback = 1.0},
        Fields::Field{.Code = 147, .Member = &Dimstyle::Dimgap, .Fallback = Mark},
        Fields::Field{.Code = 148, .Member = &Dimstyle::Dimaltrnd, .Fallback = 0.0},
        Fields::Field{.Code = 170, .Member = &Dimstyle::Dimalt, .Fallback = 0},
        Fields::Field{.Code = 171, .Member = &Dimstyle::Dimaltd, .Fallback = 2},
        Fields::Field{.Code = 172, .Member = &Dimstyle::Dimtofl, .Fallback = 0},
        Fields::Field{.Code = 173, .Member = &Dimstyle::Dimsah, .Fallback = 0},
        Fields::Field{.Code = 174, .Member = &Dimstyle::Dimtix, .Fallback = 0},
        Fields::Field{.Code = 175, .Member = &Dimstyle::Dimsoxd, .Fallback = 0},
        Fields::Field{.Code = 176, .Member = &Dimstyle::Dimclrd, .Fallback = 0},
        Fields::Field{.Code = 177, .Member = &Dimstyle::Dimclre, .Fallback = 0},
        Fields::Field{.Code = 178, .Member = &Dimstyle::Dimclrt, .Fallback = 0},
        Fields::Field{.Code = 179, .Member = &Dimstyle::Dimadec, .Fallback = 0},
        Fields::Field{.Code = 271, .Member = &Dimstyle::Dimdec, .Fallback = 4},
        Fields::Field{.Code = 272, .Member = &Dimstyle::Dimtdec, .Fallback = 4},
        Fields::Field{.Code = 273, .Member = &Dimstyle::Dimaltu, .Fallback = 2},
        Fields::Field{.Code = 274, .Member = &Dimstyle::Dimalttd, .Fallback = 2},
        Fields::Field{.Code = 275, .Member = &Dimstyle::Dimaunit, .Fallback = 0},
        Fields::Field{.Code = 276, .Member = &Dimstyle::Dimfrac, .Fallback = 0},
        Fields::Field{.Code = 277, .Member = &Dimstyle::Dimlunit, .Fallback = 2},
        Fields::Field{
            .Code     = 278,
            .Member   = &Dimstyle::Dimdsep,
            .Fallback = int{'.'},
        },
        Fields::Field{.Code = 279, .Member = &Dimstyle::Dimtmove, .Fallback = 0},
        Fields::Field{.Code = 280, .Member = &Dimstyle::Dimjust, .Fallback = 0},
        Fields::Field{.Code = 281, .Member = &Dimstyle::Dimsd1, .Fallback = 0},
        Fields::Field{.Code = 282, .Member = &Dimstyle::Dimsd2, .Fallback = 0},
        Fields::Field{.Code = 283, .Member = &Dimstyle::Dimtolj, .Fallback = 1},
        Fields::Field{.Code = 284, .Member = &Dimstyle::Dimtzin, .Fallback = 0},
        Fields::Field{.Code = 285, .Member = &Dimstyle::Dimaltz, .Fallback = 0},
        Fields::Field{.Code = 286, .Member = &Dimstyle::Dimalttz, .Fallback = 0},
        Fields::Field{.Code = 288, .Member = &Dimstyle::Dimupt, .Fallback = 0},
        Fields::Field{.Code = 289, .Member = &Dimstyle::Dimatfit, .Fallback = 3},
        Fields::Field{
            .Code     = 290,
            .Member   = &Dimstyle::Dimfxlon,
            .Fallback = false,
            .Since    = Version::R2007,
        },
        Fields::Field{.Code = 340, .Member = &Dimstyle::Dimtxsty, .Fallback = Handle{}},
        Fields::Field{.Code = 341, .Member = &Dimstyle::Dimldrblk, .Fallback = Handle{}},
        Fields::Field{.Code = 342, .Member = &Dimstyle::Dimblk, .Fallback = Handle{}},
        Fields::Field{.Code = 343, .Member = &Dimstyle::Dimblk1, .Fallback = Handle{}},
        Fields::Field{.Code = 344, .Member = &Dimstyle::Dimblk2, .Fallback = Handle{}},
        Fields::Field{
            .Code     = 345,
            .Member   = &Dimstyle::Dimltype,
            .Fallback = Handle{},
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 346,
            .Member   = &Dimstyle::Dimltex1,
            .Fallback = Handle{},
            .Since    = Version::R2007,
        },
        Fields::Field{
            .Code     = 347,
            .Member   = &Dimstyle::Dimltex2,
            .Fallback = Handle{},
            .Since    = Version::R2007,
        },
        Fields::Field{.Code = 371, .Member = &Dimstyle::Dimlwd, .Fallback = -2},
        Fields::Field{.Code = 372, .Member = &Dimstyle::Dimlwe, .Fallback = -2},
    };
  } // namespace Tables

  template <>
  inline constexpr int Self<Tables::Dimstyle> = 105;

  template <>
  inline constexpr auto Shapes<Tables::Dimstyle> = std::tuple{
      Tables::Opening,
      Fields::Subclass{.Marker = "AcDbDimStyleTableRecord"},
      Fields::Part{
          .Member = &Tables::Dimstyle::Record,
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
  inline constexpr std::string_view Type<Tables::Dimstyle> = "DIMSTYLE";
} // namespace DxfIO

#endif /* DXFIO_TABLES_SHAPES_DIMSTYLE_HPP */
