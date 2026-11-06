target_sources(dxfio
  PRIVATE
    blanks.cpp
    codes.cpp
    decimal.cpp
    glimpse.cpp
    real.cpp
    sentinel.cpp
    tag.cpp
    tags.cpp
    typed.cpp
    values.cpp
    whole.cpp
    writer.cpp
)

add_subdirectory(ascii)
add_subdirectory(binary)
