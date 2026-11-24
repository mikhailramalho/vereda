cmake_minimum_required(VERSION 3.24)

option(DOWNLOAD_DEPENDENCIES
       "Use an installed LLVM 21-23 under /usr/lib/llvm-*, else download the pinned LLVM; download camada"
       OFF)
# Before project(): it chooses the compiler and linker (LLVM's own clang, and
# lld for the downloaded LLVM, an LTO build).
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
if(LLVM_VERSION_MAJOR LESS 21 OR LLVM_VERSION_MAJOR GREATER 23)
  message(FATAL_ERROR "LLVM ${LLVM_PACKAGE_VERSION} found at ${LLVM_DIR}; 21 to 23 is supported")
endif()
message(STATUS "Using LLVM ${LLVM_PACKAGE_VERSION} from ${LLVM_DIR}")
find_package(camada CONFIG REQUIRED)
message(STATUS "Using camada from ${camada_DIR}")

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
