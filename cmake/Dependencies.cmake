# DOWNLOAD_DEPENDENCIES=ON finds or fetches LLVM, camada and mimalloc and
# points the build at them. LLVM, which must be LLVM 23: a LLVM_DIR given on
# the command line, else an installed /usr/lib/llvm-23, as apt.llvm.org's
# packages install it, else the pinned release, downloaded. camada: a
# camada_DIR given, else the pinned release. mimalloc: a MIMALLOC_SOURCE_DIR
# given (empty for glibc's malloc), else the pinned release's sources, which
# CMakeLists.txt builds into the binary. Downloads go to $VEREDA_DEPS_DIR when
# it is set, so that build directories can share them, else to <build>/deps,
# once each. Whichever LLVM it is, its own clang and lld become the compiler
# and linker unless the command line names a compiler: the official release
# is an LTO build (its .a files hold bitcode) that only they can link, and
# the models' bitcode must match the LLVM version anyway. OFF leaves
# LLVM_DIR, camada_DIR, MIMALLOC_SOURCE_DIR (unset: glibc's malloc) and the
# compiler to the user. Included before project().
if(NOT DOWNLOAD_DEPENDENCIES)
  return()
endif()

# Only the Linux x86_64 archives are pinned; add a URL and hash per
# platform when someone builds elsewhere.
cmake_host_system_information(RESULT host_os QUERY OS_NAME)
cmake_host_system_information(RESULT host_cpu QUERY OS_PLATFORM)
if(NOT host_os STREQUAL "Linux" OR NOT host_cpu STREQUAL "x86_64")
  message(FATAL_ERROR
    "DOWNLOAD_DEPENDENCIES only pins Linux x86_64 archives; install LLVM and camada and set LLVM_DIR and camada_DIR")
endif()

set(LLVM_RELEASE_NAME "LLVM-23.1.6-Linux-X64")
set(LLVM_URL "https://github.com/llvm/llvm-project/releases/download/llvmorg-23.1.6/${LLVM_RELEASE_NAME}.tar.xz")
set(LLVM_SHA256 "bd4d1c434ff69ef06d19e7934c04708da677e5f2eb51f5bcf052ab91818f0094")
set(CAMADA_URL "https://github.com/mikhailramalho/camada/releases/download/v0.26/camada-linux-x64.tar.gz")
set(CAMADA_SHA256 "fb27eb1d17d98a3e3aef579efb28ead327e0f696b99a730f46170e577162631e")
set(MIMALLOC_VERSION "3.7.0")
set(MIMALLOC_URL "https://github.com/microsoft/mimalloc/archive/refs/tags/v${MIMALLOC_VERSION}.tar.gz")
set(MIMALLOC_SHA256 "1b8653b9ff9b841a97f2c50fd996e89e1a0cb712c3c5b0980ef16a027eb7acf8")

if(DEFINED ENV{VEREDA_DEPS_DIR})
  set(DEPS_DIR "$ENV{VEREDA_DEPS_DIR}")
else()
  set(DEPS_DIR "${CMAKE_BINARY_DIR}/deps")
endif()

# Downloads and extracts one archive into ${DEPS_DIR}/<name>. <marker> is a
# path under it that exists only after a complete extraction. A stamp holds
# the extracted archive's hash, so a rerun is free and a new pin re-extracts.
# The archive is kept: file(DOWNLOAD) skips a download whose file already
# matches the hash.
function(download_dep name url sha256 marker)
  set(prefix "${DEPS_DIR}/${name}")
  set(stamp "${DEPS_DIR}/${name}.sha256")
  if(EXISTS "${prefix}/${marker}" AND EXISTS "${stamp}")
    file(READ "${stamp}" extracted)
    if(extracted STREQUAL sha256)
      return()
    endif()
  endif()
  get_filename_component(archive_name "${url}" NAME)
  set(archive "${DEPS_DIR}/${archive_name}")
  message(STATUS "Downloading ${name} from ${url}")
  file(DOWNLOAD "${url}" "${archive}" EXPECTED_HASH SHA256=${sha256}
       SHOW_PROGRESS STATUS status)
  list(GET status 0 rc)
  if(NOT rc EQUAL 0)
    file(REMOVE "${archive}")
    message(FATAL_ERROR "Failed to download ${name}: ${status}")
  endif()
  message(STATUS "Extracting ${archive_name}")
  file(REMOVE_RECURSE "${prefix}")
  file(ARCHIVE_EXTRACT INPUT "${archive}" DESTINATION "${prefix}")
  if(NOT EXISTS "${prefix}/${marker}")
    message(FATAL_ERROR "${archive_name} has no ${marker}")
  endif()
  file(WRITE "${stamp}" "${sha256}")
endfunction()

set(installed_llvm "/usr/lib/llvm-23")
if(LLVM_DIR)
  message(STATUS "LLVM_DIR given: no LLVM to download")
  get_filename_component(llvm_prefix "${LLVM_DIR}/../../.." ABSOLUTE)
elseif(EXISTS "${installed_llvm}/lib/cmake/llvm/LLVMConfig.cmake"
       AND EXISTS "${installed_llvm}/bin/clang")
  message(STATUS "Using the installed LLVM in ${installed_llvm}")
  set(llvm_prefix "${installed_llvm}")
  set(LLVM_DIR "${llvm_prefix}/lib/cmake/llvm" CACHE PATH "LLVM package directory" FORCE)
else()
  download_dep(llvm "${LLVM_URL}" "${LLVM_SHA256}"
               "${LLVM_RELEASE_NAME}/lib/cmake/llvm/LLVMConfig.cmake")
  set(llvm_prefix "${DEPS_DIR}/llvm/${LLVM_RELEASE_NAME}")
  set(LLVM_DIR "${llvm_prefix}/lib/cmake/llvm" CACHE PATH "LLVM package directory" FORCE)
endif()
if(NOT CMAKE_CXX_COMPILER AND EXISTS "${llvm_prefix}/bin/clang++")
  set(CMAKE_C_COMPILER "${llvm_prefix}/bin/clang")
  set(CMAKE_CXX_COMPILER "${llvm_prefix}/bin/clang++")
  # An LTO build's link runs ThinLTO's code generation over every LLVM
  # module the binary uses; the cache keeps those objects between links, so
  # a relink after a change to Vereda regenerates only what changed (55 s to
  # 10 s per link on an 80-core machine, for a 63 MB cache). With plain
  # objects, as apt.llvm.org's, lld just links fastest.
  if(EXISTS "${llvm_prefix}/bin/ld.lld")
    set(CMAKE_EXE_LINKER_FLAGS_INIT
        "-fuse-ld=lld -Wl,--thinlto-cache-dir=${CMAKE_BINARY_DIR}/thinlto-cache")
  endif()
endif()

if(camada_DIR)
  message(STATUS "camada_DIR given: no camada to download")
else()
  download_dep(camada "${CAMADA_URL}" "${CAMADA_SHA256}"
               "lib/cmake/camada/camadaConfig.cmake")
  set(camada_DIR "${DEPS_DIR}/camada/lib/cmake/camada"
      CACHE PATH "camada package directory" FORCE)
endif()

if(DEFINED MIMALLOC_SOURCE_DIR)
  message(STATUS "MIMALLOC_SOURCE_DIR given: no mimalloc to download")
else()
  download_dep(mimalloc "${MIMALLOC_URL}" "${MIMALLOC_SHA256}"
               "mimalloc-${MIMALLOC_VERSION}/CMakeLists.txt")
  set(MIMALLOC_SOURCE_DIR "${DEPS_DIR}/mimalloc/mimalloc-${MIMALLOC_VERSION}"
      CACHE PATH "mimalloc source directory" FORCE)
endif()
