cmake_minimum_required(VERSION 3.24)

option(DOWNLOAD_DEPENDENCIES
       "Use installed LLVM 23 and rustc 1.100.0, else download them; download camada and mimalloc"
       OFF)
# Before project(): it chooses the compiler and linker (LLVM's own clang and
# lld).
include(cmake/Dependencies.cmake)

project(vereda VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

find_package(LLVM CONFIG REQUIRED)
# A build directory configured for an older LLVM keeps it in its cache, and
# its compiler with it: such a directory has to be configured afresh.
if(NOT LLVM_VERSION_MAJOR EQUAL 23)
  message(FATAL_ERROR "LLVM ${LLVM_PACKAGE_VERSION} found at ${LLVM_DIR}; Vereda needs LLVM 23. "
                      "Set LLVM_DIR to an LLVM 23, or configure a new build directory.")
endif()
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION} from ${LLVM_DIR}")
find_package(camada CONFIG REQUIRED)
message(STATUS "Using camada from ${camada_DIR}")
include(cmake/Rust.cmake)

llvm_map_components_to_libnames(llvm_libs core irreader support passes linker)

# One static library for every stage; the driver and the unit tests link it.
add_library(libvereda STATIC
  src/analysis/loops.cpp
  src/ir/load.cpp
  src/ir/source.cpp
  src/encode/encoder.cpp
  src/encode/terms.cpp
  src/memory/memory.cpp
  src/walker/walker.cpp
  src/solver/solver.cpp
  src/witness/counterexample.cpp
  src/witness/witness.cpp)
set_target_properties(libvereda PROPERTIES OUTPUT_NAME vereda)
target_include_directories(libvereda PUBLIC src)
target_include_directories(libvereda SYSTEM PUBLIC ${LLVM_INCLUDE_DIRS})
separate_arguments(llvm_definitions NATIVE_COMMAND "${LLVM_DEFINITIONS}")
target_compile_definitions(libvereda PUBLIC ${llvm_definitions}
  VEREDA_VERSION="${PROJECT_VERSION}" VEREDA_CAMADA_VERSION="${camada_VERSION}")
if(NOT LLVM_ENABLE_RTTI)
  target_compile_options(libvereda PUBLIC -fno-rtti)
endif()
target_link_libraries(libvereda PUBLIC ${llvm_libs} camada::camada)

add_executable(vereda src/driver/main.cpp)
target_link_libraries(vereda PRIVATE libvereda)
# Recheck HEAD on each build, rewriting the header only when it changes.
add_custom_target(vereda-version
  COMMAND "${CMAKE_COMMAND}" "-DSOURCE_DIR=${CMAKE_SOURCE_DIR}"
          "-DOUTPUT=${CMAKE_BINARY_DIR}/generated/vereda-version.h"
          -P "${CMAKE_SOURCE_DIR}/cmake/Version.cmake"
  BYPRODUCTS "${CMAKE_BINARY_DIR}/generated/vereda-version.h"
  VERBATIM)
add_dependencies(vereda vereda-version)
target_include_directories(vereda PRIVATE "${CMAKE_BINARY_DIR}/generated")
# ld.bfd's default script gathers the static constructors (.text.startup)
# in one place; lld leaves each with its object, so the 1,789 of Z3, cvc5
# and LLVM run from 921 separate 64 KiB windows of .text, and the kernel
# maps each window whole: 79 MB resident on --version instead of 34 MB
# (docs/survey/2026-11-24-lld-startup-memory.md). With
# -z keep-text-section-prefix lld gathers them too. CMake before 3.29 does
# not say which linker it uses, and the link stays as it was.
if(CMAKE_CXX_COMPILER_LINKER_ID STREQUAL "LLD")
  target_link_options(vereda PRIVATE LINKER:-z,keep-text-section-prefix)
endif()

# mimalloc in place of glibc's malloc and operator new, for all of the
# binary (LLVM, the solvers): it cuts the walk and solve times by up to a
# third (docs/survey/2026-11-23-mimalloc.md). Linked as ESBMC links it: its
# object file, since with MI_USE_CXX the archive fails to link, and
# MI_USE_CXX so that operator new throws std::bad_alloc instead of aborting.
# The tests cap address space (RLIMIT_AS), and mimalloc keeps the address
# space of what it frees, so two defaults change, both built in because
# LLVM's static constructors allocate before main() could set them. Arenas
# start at 64 MiB instead of 1 GiB, as in ESBMC. Objects over 32 MiB get
# their own mapping, unmapped when freed, as glibc does, instead of an arena
# of their own that outlives them: on esbmc-cstd/scanf-false-3, CaDiCaL
# growing a vector to 512 MiB ran out of address space at 2.4 GB resident.
# Without a MIMALLOC_SOURCE_DIR (DOWNLOAD_DEPENDENCIES sets one), glibc's
# malloc stays.
if(MIMALLOC_SOURCE_DIR)
  set(MI_BUILD_SHARED OFF CACHE BOOL "" FORCE)
  set(MI_BUILD_STATIC OFF CACHE BOOL "" FORCE)
  set(MI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(MI_USE_CXX ON CACHE STRING "" FORCE)
  add_subdirectory("${MIMALLOC_SOURCE_DIR}" mimalloc EXCLUDE_FROM_ALL)
  target_compile_definitions(mimalloc-obj PRIVATE # KiB
    MI_DEFAULT_ARENA_RESERVE=65536 MI_DEFAULT_ARENA_MAX_OBJECT_SIZE=32768)
  target_sources(vereda PRIVATE $<TARGET_OBJECTS:mimalloc-obj>)
endif()

# The C library models (models/*.c) as one bitcode file per pointer width,
# next to the binary, already through the pass list ir runs; ir links in
# the functions a program still calls after its own passes. -fno-builtin
# keeps InstCombine from rewriting calls inside the models. No -g: without
# debug records a model's assignments stay out of counterexamples, as
# ESBMC's __ESBMC_HIDE label keeps its library's out (tests/vereda/
# lib_hidden.c).
file(GLOB model_sources CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/models/*.c")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/models")
foreach(bits 32 64)
  set(model_bcs)
  foreach(src IN LISTS model_sources)
    get_filename_component(name "${src}" NAME_WE)
    set(bc "${CMAKE_BINARY_DIR}/models/${name}-${bits}.bc")
    add_custom_command(OUTPUT "${bc}"
      COMMAND "${LLVM_TOOLS_BINARY_DIR}/clang" -O0 -Xclang -disable-O0-optnone -emit-llvm -c -fno-builtin
              -fno-finite-loops -m${bits} "${src}" -o "${bc}.raw"
      COMMAND "${LLVM_TOOLS_BINARY_DIR}/opt" "${bc}.raw" -o "${bc}"
              "-passes=function(mem2reg,sroa,instcombine<no-verify-fixpoint>,scalarizer<load-store;variable-insert-extract>,loop-simplify,lcssa)"
      DEPENDS "${src}" VERBATIM)
    list(APPEND model_bcs "${bc}")
  endforeach()
  add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/models-${bits}.bc"
    COMMAND "${LLVM_TOOLS_BINARY_DIR}/llvm-link" ${model_bcs} -o "${CMAKE_BINARY_DIR}/models-${bits}.bc"
    DEPENDS ${model_bcs} VERBATIM)
  list(APPEND model_libs "${CMAKE_BINARY_DIR}/models-${bits}.bc")
endforeach()
add_custom_target(models ALL DEPENDS ${model_libs})

enable_testing()
add_subdirectory(tests)
