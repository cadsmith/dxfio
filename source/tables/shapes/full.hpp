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
    Fields::Field{.Code = 179, .Member = &Dimstyle::Dimadec},
    Fields::Field{.Code = 271, .Member = &Dimstyle::Dimdec},
    Fields::Field{.Code = 272, .Member = &Dimstyle::Dimtdec},
    Fields::Field{.Code = 273, .Member = &Dimstyle::Dimaltu},
    Fields::Field{.Code = 274, .Member = &Dimstyle::Dimalttd},
    Fields::Field{.Code = 275, .Member = &Dimstyle::Dimaunit},
    Fields::Field{.Code = 276, .Member = &Dimstyle::Dimfrac},
    Fields::Field{.Code = 277, .Member = &Dimstyle::Dimlunit},
    Fields::Field{.Code = 278, .Member = &Dimstyle::Dimdsep},
    Fields::Field{.Code = 280, .Member = &Dimstyle::Dimjust},
    Fields::Field{.Code = 281, .Member = &Dimstyle::Dimsd1},
    Fields::Field{.Code = 282, .Member = &Dimstyle::Dimsd2},
    Fields::Field{.Code = 283, .Member = &Dimstyle::Dimtolj},
    Fields::Field{.Code = 284, .Member = &Dimstyle::Dimtzin},
    Fields::Field{.Code = 285, .Member = &Dimstyle::Dimaltz},
    Fields::Field{.Code = 286, .Member = &Dimstyle::Dimalttz},
    Fields::Field{.Code = 288, .Member = &Dimstyle::Dimupt},
    Fields::Field{.Code = 340, .Member = &Dimstyle::Dimtxsty},
    Fields::Field{.Code = 341, .Member = &Dimstyle::Dimldrblk, .Fallback = Handle{}},
    Fields::Field{.Code = 342, .Member = &Dimstyle::Dimblk, .Fallback = Handle{}},
    Fields::Field{.Code = 343, .Member = &Dimstyle::Dimblk1, .Fallback = Handle{}},
    Fields::Field{.Code = 344, .Member = &Dimstyle::Dimblk2, .Fallback = Handle{}},
};

inline constexpr auto Full = std::tuple{
    Fields::Field{.Code = 3, .Member = &Dimstyle::Dimpost},
    Fields::Field{.Code = 4, .Member = &Dimstyle::Dimapost},
    Fields::Part{.Member = &Dimstyle::Arrows, .Fields = Names},
    Fields::Field{.Code = 40, .Member = &Dimstyle::Dimscale},
    Fields::Field{.Code = 41, .Member = &Dimstyle::Dimasz},
    Fields::Field{.Code = 42, .Member = &Dimstyle::Dimexo},
    Fields::Field{.Code = 43, .Member = &Dimstyle::Dimdli},
    Fields::Field{.Code = 44, .Member = &Dimstyle::Dimexe},
    Fields::Field{.Code = 45, .Member = &Dimstyle::Dimrnd},
    Fields::Field{.Code = 46, .Member = &Dimstyle::Dimdle},
    Fields::Field{.Code = 47, .Member = &Dimstyle::Dimtp},
    Fields::Field{.Code = 48, .Member = &Dimstyle::Dimtm},
    Fields::Field{.Code = 140, .Member = &Dimstyle::Dimtxt},
    Fields::Field{.Code = 141, .Member = &Dimstyle::Dimcen},
    Fields::Field{.Code = 142, .Member = &Dimstyle::Dimtsz},
    Fields::Field{.Code = 143, .Member = &Dimstyle::Dimaltf},
    Fields::Field{.Code = 144, .Member = &Dimstyle::Dimlfac},
    Fields::Field{.Code = 145, .Member = &Dimstyle::Dimtvp},
    Fields::Field{.Code = 146, .Member = &Dimstyle::Dimtfac},
    Fields::Field{.Code = 147, .Member = &Dimstyle::Dimgap},
    Fields::Field{.Code = 71, .Member = &Dimstyle::Dimtol},
    Fields::Field{.Code = 72, .Member = &Dimstyle::Dimlim},
    Fields::Field{.Code = 73, .Member = &Dimstyle::Dimtih},
    Fields::Field{.Code = 74, .Member = &Dimstyle::Dimtoh},
    Fields::Field{.Code = 75, .Member = &Dimstyle::Dimse1},
    Fields::Field{.Code = 76, .Member = &Dimstyle::Dimse2},
    Fields::Field{.Code = 77, .Member = &Dimstyle::Dimtad},
    Fields::Field{.Code = 78, .Member = &Dimstyle::Dimzin},
    Fields::Field{.Code = 170, .Member = &Dimstyle::Dimalt},
    Fields::Field{.Code = 171, .Member = &Dimstyle::Dimaltd},
    Fields::Field{.Code = 172, .Member = &Dimstyle::Dimtofl},
    Fields::Field{.Code = 173, .Member = &Dimstyle::Dimsah},
    Fields::Field{.Code = 174, .Member = &Dimstyle::Dimtix},
    Fields::Field{.Code = 175, .Member = &Dimstyle::Dimsoxd},
    Fields::Field{.Code = 176, .Member = &Dimstyle::Dimclrd},
    Fields::Field{.Code = 177, .Member = &Dimstyle::Dimclre},
    Fields::Field{.Code = 178, .Member = &Dimstyle::Dimclrt},
    Fields::Since{.Release = Version::R13, .Fields = Added},
};
} // namespace DxfIO::Tables

#endif /* DXFIO_TABLES_SHAPES_FULL_HPP */
