add_library(dxfio
  braced.cpp
  braces.cpp
  create.cpp
  entity.cpp
  handle.cpp
  handseed.cpp
  head.cpp
  load.cpp
  object.cpp
  past.cpp
  raw.cpp
  save.cpp
  seed.cpp
  symbol.cpp
  tail.cpp
  unknown.cpp
  version.cpp
)

add_subdirectory(audit)
add_subdirectory(blocks)
add_subdirectory(down)
add_subdirectory(edit)
add_subdirectory(empty)
add_subdirectory(encoding)
add_subdirectory(entities)
add_subdirectory(fields)
add_subdirectory(file)
add_subdirectory(header)
add_subdirectory(objects)
add_subdirectory(plan)
add_subdirectory(tables)
add_subdirectory(tags)

add_library(dxfio::dxfio ALIAS dxfio)

target_include_directories(dxfio
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_SOURCE_DIR}/source
)

target_compile_features(dxfio PUBLIC cxx_std_23)

set_target_properties(dxfio PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN YES
)

get_target_property(dxf_type dxfio TYPE)
if(dxf_type STREQUAL "STATIC_LIBRARY")
  target_compile_definitions(dxfio PUBLIC DXFIO_STATIC)
endif()

if (DXFIO_ENABLE_TIDY)
  find_program(DXFIO_CLANG_TIDY clang-tidy REQUIRED
    PATHS /opt/homebrew/opt/llvm/bin /usr/local/opt/llvm/bin)
  set_target_properties(dxfio PROPERTIES CXX_CLANG_TIDY ${DXFIO_CLANG_TIDY})
endif()
