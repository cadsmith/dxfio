include("${CMAKE_CURRENT_LIST_DIR}/dxfioTargets.cmake")

foreach(config IN ITEMS MINSIZEREL RELWITHDEBINFO)
  if (NOT DEFINED CMAKE_MAP_IMPORTED_CONFIG_${config})
    set_property(TARGET dxfio::dxfio
      PROPERTY MAP_IMPORTED_CONFIG_${config} Release Debug
    )
  endif()
endforeach()
