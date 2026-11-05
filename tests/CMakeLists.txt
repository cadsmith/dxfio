add_executable(dxfio_tests
  main.cpp
  fuzz/oracle.cpp
)

target_link_libraries(dxfio_tests
  PRIVATE
    doctest::doctest
)

target_compile_features(dxfio_tests PRIVATE cxx_std_23)

target_compile_definitions(dxfio_tests
  PRIVATE
    DOCTEST_CONFIG_USE_STD_HEADERS
    DXFIO_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
    DXFIO_FUZZ_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fuzz"
)

target_include_directories(dxfio_tests
  PRIVATE
    ${PROJECT_SOURCE_DIR}/source
)

add_subdirectory(api)
add_subdirectory(dxfio)

get_target_property(units dxfio_tests SOURCES)

foreach(unit IN LISTS units)
  cmake_path(ABSOLUTE_PATH unit BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  file(RELATIVE_PATH name ${CMAKE_CURRENT_SOURCE_DIR}/dxfio ${unit})
  if (name MATCHES "^\\.\\.")
    continue()
  endif()
  string(REGEX REPLACE "\\.cpp$" "" name ${name})
  string(REPLACE "/" "?" file ${name})
  add_test(NAME dxfio/${name}
    COMMAND dxfio_tests "-sf=*tests?dxfio?${file}.cpp")
  set_tests_properties(dxfio/${name} PROPERTIES
    FAIL_REGULAR_EXPRESSION "test cases: +0 \\|")
endforeach()

if (DXFIO_ENABLE_FUZZ)
  add_subdirectory(fuzz)
endif()

if (DXFIO_ENABLE_COVERAGE)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(LLVM_PROFDATA xcrun llvm-profdata)
    set(LLVM_COV xcrun llvm-cov)
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
    find_program(LLVM_PROFDATA llvm-profdata HINTS ${COMPILER_DIR})
    find_program(LLVM_COV llvm-cov HINTS ${COMPILER_DIR})
  endif()

  if (LLVM_PROFDATA AND LLVM_COV)
    set(COVERAGE_SOURCES
      ${PROJECT_SOURCE_DIR}/include
      ${PROJECT_SOURCE_DIR}/source
    )
    add_custom_target(coverage
      COMMAND
        ${CMAKE_COMMAND} -E env
        LLVM_PROFILE_FILE=dxfio_tests.profraw
        $<TARGET_FILE:dxfio_tests>
      COMMAND
        ${LLVM_PROFDATA} merge -sparse
        dxfio_tests.profraw -o dxfio_tests.profdata
      COMMAND
        ${LLVM_COV} report $<TARGET_FILE:dxfio_tests>
        -instr-profile=dxfio_tests.profdata
        -show-branch-summary
        ${COVERAGE_SOURCES}
      COMMAND
        ${LLVM_COV} show $<TARGET_FILE:dxfio_tests>
        -instr-profile=dxfio_tests.profdata
        -format=html -output-dir=coverage_html
        ${COVERAGE_SOURCES}
      COMMAND
        ${CMAKE_COMMAND}
        "-DCOVERAGE_TOOL=${LLVM_COV}"
        -DCOVERAGE_BINARY=$<TARGET_FILE:dxfio_tests>
        -DCOVERAGE_PROFILE=dxfio_tests.profdata
        "-DCOVERAGE_SOURCES=${COVERAGE_SOURCES}"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/coverage.cmake
      WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
      DEPENDS
        dxfio_tests
      VERBATIM
    )
  endif()
endif()
