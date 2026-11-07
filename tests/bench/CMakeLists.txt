add_executable(dxfio_bench EXCLUDE_FROM_ALL
  main.cpp
)

target_link_libraries(dxfio_bench
  PRIVATE
    dxfio::dxfio
    $<$<PLATFORM_ID:Windows>:psapi>
)

target_compile_features(dxfio_bench PRIVATE cxx_std_23)
