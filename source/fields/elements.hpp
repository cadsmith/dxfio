#ifndef DXFIO_FIELDS_ELEMENTS_HPP
#define DXFIO_FIELDS_ELEMENTS_HPP

#include "fields/walk.hpp"

namespace DxfIO::Fields
{
  template <class StepT>
  void Elements(Walk &in, int opener, StepT step)
  {
    in.Expect(opener);
    in.Rewind();
    const Walk::Fence outer = in.Fenced();
    for (in.Narrow(opener, outer); in.Fenced().Begin < outer.End; in.Narrow(opener, outer))
    {
      const std::size_t begin = in.Fenced().Begin;
      step();
      in.Restore(Walk::Fence{
          .Begin  = outer.Begin,
          .End    = outer.End,
          .Cursor = begin + 1,
      });
    }
    in.Restore(outer);
  }
} // namespace DxfIO::Fields

#endif /* DXFIO_FIELDS_ELEMENTS_HPP */
