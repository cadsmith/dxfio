set(DOCTEST_WITH_TESTS OFF CACHE BOOL "")
set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF CACHE BOOL "")
set(DOCTEST_NO_INSTALL ON CACHE BOOL "")

FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG        v2.5.3
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
  SYSTEM
  FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(doctest)
