set(FETCHCONTENT_QUIET OFF)

include(FetchContent)

if (BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
  add_subdirectory(doctest)
endif()
