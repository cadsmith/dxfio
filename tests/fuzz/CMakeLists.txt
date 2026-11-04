foreach(target dxf escapes tags)
  add_executable(dxfio_fuzz_${target}
    ${target}.cpp
    oracle.cpp
  )

  target_link_libraries(dxfio_fuzz_${target}
    PRIVATE
      dxfio::dxfio
  )

  target_include_directories(dxfio_fuzz_${target}
    PRIVATE
      ${PROJECT_SOURCE_DIR}/source
  )

  target_compile_features(dxfio_fuzz_${target} PRIVATE cxx_std_23)

  target_link_options(dxfio_fuzz_${target} PRIVATE -fsanitize=fuzzer)
endforeach()
