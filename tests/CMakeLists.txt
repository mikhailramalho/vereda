# Every *.c, *.i, *.rs or *.ll directly under a suite directory is a test, and so is
# every <suite>/<name>/main.c for tests with several files. The test's comment
# header describes it; see run.py. The suite name is the ctest label, so
# `ctest -L vereda` runs one suite.
# A test past this is skipped; see run.py.
set(REGRESS_TIMEOUT 300 CACHE STRING "Per-test timeout in seconds")
# A test past this fails, unless KNOWNBUG: a CI runner that runs out of memory
# stops answering instead of failing the job.
set(REGRESS_MEMORY_LIMIT 4096 CACHE STRING "Per-test memory limit in MB (address space)")

add_test(NAME vereda/output_readers COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/output_readers.py")
set_tests_properties(vereda/output_readers PROPERTIES LABELS vereda)
add_executable(vereda-table-patterns table_patterns.cpp)
target_link_libraries(vereda-table-patterns PRIVATE libvereda)
add_test(NAME vereda/table_patterns COMMAND vereda-table-patterns)
set_tests_properties(vereda/table_patterns PROPERTIES LABELS vereda TIMEOUT 60)
add_test(NAME vereda/banner
  COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/banner.py"
          "${CMAKE_SOURCE_DIR}/vereda.py" $<TARGET_FILE:vereda> "${LLVM_TOOLS_BINARY_DIR}/clang")
set_tests_properties(vereda/banner PROPERTIES LABELS vereda)

file(GLOB suites LIST_DIRECTORIES true RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
     "${CMAKE_CURRENT_SOURCE_DIR}/*")
foreach(suite IN LISTS suites)
  if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${suite}")
    continue()
  endif()
  file(GLOB tests LIST_DIRECTORIES false CONFIGURE_DEPENDS
       "${CMAKE_CURRENT_SOURCE_DIR}/${suite}/*.c"
       "${CMAKE_CURRENT_SOURCE_DIR}/${suite}/*.i"
       "${CMAKE_CURRENT_SOURCE_DIR}/${suite}/*.ll"
       "${CMAKE_CURRENT_SOURCE_DIR}/${suite}/*.rs"
       "${CMAKE_CURRENT_SOURCE_DIR}/${suite}/*/main.c")
  foreach(test IN LISTS tests)
    # The last extension only: some test names hold dots.
    get_filename_component(name "${test}" NAME_WLE)
    if(name STREQUAL "main")
      get_filename_component(name "${test}" DIRECTORY)
      get_filename_component(name "${name}" NAME)
    endif()
    add_test(NAME "${suite}/${name}"
             COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/run.py"
                     --vereda "${CMAKE_SOURCE_DIR}/vereda.py" --bin $<TARGET_FILE:vereda>
                     --clang "${LLVM_TOOLS_BINARY_DIR}/clang" --timeout "${REGRESS_TIMEOUT}"
                     --memory-limit "${REGRESS_MEMORY_LIMIT}" "${test}")
    # run.py stops the run itself and reports it as skipped; ctest's limit is
    # only a backstop.
    math(EXPR backstop "${REGRESS_TIMEOUT} + 60")
    # run.py exits 77 for a KNOWNBUG or FUTURE test that fails as expected.
    set_tests_properties("${suite}/${name}" PROPERTIES LABELS "${suite}" TIMEOUT "${backstop}"
                         SKIP_RETURN_CODE 77)
  endforeach()
endforeach()
