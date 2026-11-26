# Every *.c, *.i, *.rs or *.ll directly under a suite directory is a test, and so is
# every <suite>/<name>/main.c for tests with several files. The test's comment
# header describes it; see run.py. The suite name is the ctest label, so
# `ctest -L vereda` runs one suite.
# A test past this is skipped; see run.py.
set(REGRESS_TIMEOUT 300 CACHE STRING "Per-test timeout in seconds")
# A test past this fails, unless KNOWNBUG: a CI runner that runs out of memory
# stops answering instead of failing the job.
set(REGRESS_MEMORY_LIMIT 4096 CACHE STRING "Per-test memory limit in MB (address space)")

# The registration-only fixture has no verifier library or C++ toolchain.
if(TARGET libvereda)
  add_test(NAME vereda/output_readers COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/output_readers.py")
  set_tests_properties(vereda/output_readers PROPERTIES LABELS vereda)
  add_test(NAME vereda/test_configurations COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/test_configurations.py")
  set_tests_properties(vereda/test_configurations PROPERTIES LABELS vereda TIMEOUT 30)
  add_executable(vereda-table-patterns table_patterns.cpp)
  target_link_libraries(vereda-table-patterns PRIVATE libvereda)
  add_test(NAME vereda/table_patterns COMMAND vereda-table-patterns)
  set_tests_properties(vereda/table_patterns PROPERTIES LABELS vereda TIMEOUT 60)
  add_executable(vereda-store-reads store_reads.cpp)
  target_link_libraries(vereda-store-reads PRIVATE libvereda)
  add_test(NAME vereda/store_reads COMMAND vereda-store-reads)
  set_tests_properties(vereda/store_reads PROPERTIES LABELS vereda TIMEOUT 60)
  add_test(NAME vereda/banner
    COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/banner.py"
            "${CMAKE_SOURCE_DIR}/vereda.py" $<TARGET_FILE:vereda> "${LLVM_TOOLS_BINARY_DIR}/clang")
  set_tests_properties(vereda/banner PROPERTIES LABELS vereda)
  add_test(NAME vereda/check_flags
    COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/check_flags.py"
            "${CMAKE_SOURCE_DIR}/vereda.py" $<TARGET_FILE:vereda> "${LLVM_TOOLS_BINARY_DIR}/clang")
  set_tests_properties(vereda/check_flags PROPERTIES LABELS vereda TIMEOUT 60)
  add_test(NAME vereda/frontend_options
    COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/frontend_options.py"
            "${CMAKE_SOURCE_DIR}/vereda.py" $<TARGET_FILE:vereda> "${LLVM_TOOLS_BINARY_DIR}/clang")
  set_tests_properties(vereda/frontend_options PROPERTIES LABELS vereda TIMEOUT 60)
endif()

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
    # Reconfigure when RUN lines change, not only when files are added.
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${test}")
    file(STRINGS "${test}" runs REGEX "^[ \t]*(//|;)[ \t]*RUN:")
    list(LENGTH runs run_count)
    if(run_count EQUAL 0)
      set(run_count 1)
    endif()
    # run.py stops the run itself and reports it as skipped; ctest's limit is
    # only a backstop.
    math(EXPR backstop "${REGRESS_TIMEOUT} + 60")
    # run.py exits 77 for a KNOWNBUG or FUTURE test that fails as expected.
    foreach(run_index RANGE 1 ${run_count})
      set(run_name "${suite}/${name}")
      if(run_index GREATER 1)
        string(APPEND run_name "@run${run_index}")
      endif()
      add_test(NAME "${run_name}"
               COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/run.py"
                       --vereda "${CMAKE_SOURCE_DIR}/vereda.py" --bin $<TARGET_FILE:vereda>
                       --clang "${LLVM_TOOLS_BINARY_DIR}/clang" --timeout "${REGRESS_TIMEOUT}"
                       --memory-limit "${REGRESS_MEMORY_LIMIT}" --run-index "${run_index}"
                       "${test}")
      set_tests_properties("${run_name}" PROPERTIES LABELS "${suite}" TIMEOUT "${backstop}"
                           SKIP_RETURN_CODE 77)
    endforeach()
  endforeach()
endforeach()
