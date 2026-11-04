#ifndef DXFIO_TESTS_FUZZ_ORACLE_HPP
#define DXFIO_TESTS_FUZZ_ORACLE_HPP

#include <string>
#include <string_view>

namespace DxfIO
{
auto Broken(std::string_view bytes) -> std::string;
} // namespace DxfIO

#endif /* DXFIO_TESTS_FUZZ_ORACLE_HPP */
