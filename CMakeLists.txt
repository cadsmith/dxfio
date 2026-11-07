cmake_minimum_required(VERSION 3.28)
project(dxfio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

option(DXFIO_ENABLE_COVERAGE "Instrument for LLVM coverage" OFF)
if (DXFIO_ENABLE_COVERAGE)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping -fprofile-update=atomic)
    add_link_options(-fprofile-instr-generate)
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(--coverage -fprofile-update=atomic)
    add_link_options(--coverage)
  endif()
endif()

option(DXFIO_ENABLE_FUZZ "Build the libFuzzer targets" OFF)
if (DXFIO_ENABLE_FUZZ)
  add_compile_options(-fsanitize=fuzzer-no-link)
endif()

option(DXFIO_ENABLE_SANITIZE "Instrument for ASan and UBSan" OFF)
if (DXFIO_ENABLE_SANITIZE)
  set(DXFIO_SANITIZERS address,undefined,float-divide-by-zero)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    string(APPEND DXFIO_SANITIZERS
      ,implicit-conversion,local-bounds,nullability)
    add_compile_options(
      -fsanitize-ignorelist=${PROJECT_SOURCE_DIR}/tests/ignorelist.txt
    )
  endif()
  add_compile_options(
    -fsanitize=${DXFIO_SANITIZERS}
    -fno-sanitize-recover=all
    -fno-omit-frame-pointer
  )
  add_link_options(-fsanitize=${DXFIO_SANITIZERS})
endif()

option(DXFIO_ENABLE_THREAD "Instrument for TSan" OFF)
if (DXFIO_ENABLE_THREAD)
  add_compile_options(
    -fsanitize=thread
    -fno-sanitize-recover=all
    -fno-omit-frame-pointer
  )
  add_link_options(-fsanitize=thread)
endif()

option(DXFIO_ENABLE_MEMORY "Instrument for MSan, with DXFIO_MEMORY_LIBCXX" OFF)
set(DXFIO_MEMORY_LIBCXX "" CACHE PATH "A libc++ built with MSan")
if (DXFIO_ENABLE_MEMORY)
  add_compile_options(
    -fsanitize=memory
    -fsanitize-memory-track-origins
    -fno-sanitize-recover=all
    -fno-omit-frame-pointer
    -nostdinc++
    -isystem ${DXFIO_MEMORY_LIBCXX}/include/c++/v1
  )
  add_link_options(
    -fsanitize=memory
    -stdlib=libc++
    -L${DXFIO_MEMORY_LIBCXX}/lib
    -Wl,-rpath,${DXFIO_MEMORY_LIBCXX}/lib
    -lc++abi
  )
endif()

option(DXFIO_ENABLE_TIDY "Run clang-tidy on the library as it builds" OFF)

if (PROJECT_IS_TOP_LEVEL)
  add_compile_definitions(
    _GLIBCXX_ASSERTIONS
    _LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG
  )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(
    -Wall -Wextra -Wpedantic -Wshadow
    -Wconversion -Wsign-conversion -Wold-style-cast
    -Wnon-virtual-dtor -Woverloaded-virtual
  )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wnull-dereference)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wno-maybe-uninitialized)
endif()

if (MSVC)
  add_compile_options(/W4 /utf-8)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(CTest)
endif()

add_subdirectory(vendor SYSTEM)
add_subdirectory(source)
add_subdirectory(include)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS dxfio EXPORT dxfioTargets FILE_SET HEADERS)

install(EXPORT dxfioTargets
  NAMESPACE dxfio::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dxfio
)

write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/dxfioConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
)

install(FILES ${PROJECT_SOURCE_DIR}/cmake/config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dxfio
  RENAME dxfioConfig.cmake
)

install(FILES ${PROJECT_BINARY_DIR}/dxfioConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dxfio
)

if (BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
  add_subdirectory(tests)
endif()
