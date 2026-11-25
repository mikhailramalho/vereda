find_program(GIT_EXECUTABLE git)
set(commit "unknown")
if(GIT_EXECUTABLE)
  execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${SOURCE_DIR}" rev-parse --short=8 HEAD
    OUTPUT_VARIABLE revision OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET RESULT_VARIABLE result)
  if(result EQUAL 0)
    set(commit "${revision}")
  endif()
endif()
file(CONFIGURE OUTPUT "${OUTPUT}" CONTENT
  "// Stage driver: build revision for the version banner.\n#define VEREDA_COMMIT \"${commit}\"\n")
