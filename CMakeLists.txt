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
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate)
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(--coverage)
    add_link_options(--coverage)
  endif()
endif()

option(DXFIO_ENABLE_FUZZ "Build the libFuzzer targets" OFF)
if (DXFIO_ENABLE_FUZZ)
  add_compile_options(-fsanitize=fuzzer-no-link)
endif()

option(DXFIO_ENABLE_SANITIZE "Instrument for ASan and UBSan" OFF)
if (DXFIO_ENABLE_SANITIZE)
  add_compile_options(
    -fsanitize=address,undefined
    -fno-sanitize-recover=all
    -fno-omit-frame-pointer
  )
  add_link_options(-fsanitize=address,undefined)
endif()

option(DXFIO_ENABLE_TIDY "Run clang-tidy on the library as it builds" OFF)

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
  add_compile_options(/utf-8)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(CTest)
endif()

add_subdirectory(vendor SYSTEM)
add_subdirectory(source)

if (BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
  add_subdirectory(tests)
endif()
