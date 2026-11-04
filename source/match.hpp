#ifndef DXFIO_MATCH_HPP
#define DXFIO_MATCH_HPP

namespace DxfIO
{
// The overload set of lambdas std::visit takes; inheriting from each case is the point.
template <typename... CasesT>
struct Match : CasesT... // NOLINT(misc-multiple-inheritance)
{
  using CasesT::operator()...;
};
} // namespace DxfIO

#endif /* DXFIO_MATCH_HPP */
