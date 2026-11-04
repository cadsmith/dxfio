execute_process(
  COMMAND
    ${COVERAGE_TOOL} export ${COVERAGE_BINARY}
    -instr-profile=${COVERAGE_PROFILE}
    -summary-only
    ${COVERAGE_SOURCES}
  OUTPUT_VARIABLE summary
  COMMAND_ERROR_IS_FATAL ANY
)

string(JSON files LENGTH "${summary}" data 0 files)
set(index 0)
while (index LESS files)
  string(JSON file GET "${summary}" data 0 files ${index} filename)
  set(inside OFF)
  foreach(folder IN LISTS COVERAGE_SOURCES)
    cmake_path(IS_PREFIX folder "${file}" NORMALIZE inside)
    if (inside)
      break()
    endif()
  endforeach()
  if (NOT inside)
    message(FATAL_ERROR "coverage: ${file} is outside the gated folders")
  endif()
  math(EXPR index "${index} + 1")
endwhile()

foreach(kind lines functions regions branches)
  string(JSON covered GET "${summary}" data 0 totals ${kind} covered)
  string(JSON count GET "${summary}" data 0 totals ${kind} count)
  if (count EQUAL 0 OR NOT covered EQUAL count)
    message(FATAL_ERROR "coverage: ${covered} of ${count} ${kind}")
  endif()
endforeach()
