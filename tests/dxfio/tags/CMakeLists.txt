target_sources(dxfio_tests
  PRIVATE
    codes.cpp
    decimal.cpp
    real.cpp
    sentinel.cpp
    tag.cpp
    tags.cpp
    values.cpp
    whole.cpp
    writer.cpp
)

add_subdirectory(ascii)
add_subdirectory(binary)
