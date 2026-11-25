# Rust 1.100 emits LLVM 23 bitcode. Keep one compiler version for the source
# tests; use the system compiler when it matches, otherwise install locally.
set(RUST_VERSION "1.100.0")
if(RUSTC_EXECUTABLE)
  set(rustc_candidate "${RUSTC_EXECUTABLE}")
else()
  find_program(rustc_candidate NAMES rustc)
endif()
if(rustc_candidate)
  execute_process(COMMAND "${rustc_candidate}" --version
    OUTPUT_VARIABLE rustc_version ERROR_QUIET RESULT_VARIABLE rustc_result
    OUTPUT_STRIP_TRAILING_WHITESPACE TIMEOUT 10)
endif()
if(rustc_result STREQUAL "0" AND rustc_version MATCHES "^rustc 1\\.100\\.0 ")
  set(RUSTC_EXECUTABLE "${rustc_candidate}" CACHE FILEPATH "Rust 1.100.0 compiler" FORCE)
elseif(RUSTC_EXECUTABLE OR NOT DOWNLOAD_DEPENDENCIES)
  message(FATAL_ERROR "Vereda needs rustc ${RUST_VERSION}; set RUSTC_EXECUTABLE or use DOWNLOAD_DEPENDENCIES=ON")
else()
  set(rust_target "x86_64-unknown-linux-gnu")
  set(rustc_release "rustc-${RUST_VERSION}-${rust_target}")
  set(rust_std_release "rust-std-${RUST_VERSION}-${rust_target}")
  download_dep(rustc "https://static.rust-lang.org/dist/${rustc_release}.tar.xz"
    "f1d0b1dec81dab6f523cd098507763487b0d4bd7fd2c87582f154128719be03b"
    "${rustc_release}/install.sh")
  download_dep(rust-std "https://static.rust-lang.org/dist/${rust_std_release}.tar.xz"
    "4591b6486594ded52ecd6135db95bf9355bbd3ca7a380888b141528bbcf9a5d4"
    "${rust_std_release}/install.sh")
  set(rust_prefix "${DEPS_DIR}/rust-${RUST_VERSION}")
  if(NOT EXISTS "${rust_prefix}/bin/rustc" OR
     NOT EXISTS "${rust_prefix}/.installed")
    foreach(component rustc rust-std)
      set(release "${component}-${RUST_VERSION}-${rust_target}")
      execute_process(COMMAND sh "${DEPS_DIR}/${component}/${release}/install.sh"
        "--prefix=${rust_prefix}" --disable-ldconfig
        OUTPUT_QUIET COMMAND_ERROR_IS_FATAL ANY)
    endforeach()
    file(WRITE "${rust_prefix}/.installed" "${RUST_VERSION}\n")
  endif()
  set(RUSTC_EXECUTABLE "${rust_prefix}/bin/rustc" CACHE FILEPATH "Rust 1.100.0 compiler" FORCE)
endif()
message(STATUS "Using rustc ${RUST_VERSION} from ${RUSTC_EXECUTABLE}")
# The front end can find the selected compiler beside --bin, even for a build
# outside the source tree. No downloads happen while running a verification.
file(WRITE "${CMAKE_BINARY_DIR}/rustc-path.txt" "${RUSTC_EXECUTABLE}\n")
