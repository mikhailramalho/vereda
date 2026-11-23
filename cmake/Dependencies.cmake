# DOWNLOAD_DEPENDENCIES=ON finds or fetches LLVM and camada and points
# find_package at them. LLVM: a LLVM_DIR given on the command line, else an
# installed /usr/lib/llvm-21 (22, 23), as apt.llvm.org's packages install it,
# else the pinned release, downloaded. camada: a camada_DIR given, else the
# pinned release. Downloads go to $VEREDA_DEPS_DIR when it is set, so that
# build directories can share them, else to <build>/deps, once each. The
# official LLVM release is an LTO build (its .a files hold bitcode), so its
# own clang and lld become the compiler and linker; an installed LLVM's clang
# is the compiler too, since the models' bitcode must match its version. OFF
# leaves LLVM_DIR, camada_DIR and the compiler to the user. Included before
# project().
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

set(LLVM_RELEASE_NAME "LLVM-21.1.8-Linux-X64")
set(LLVM_URL "https://github.com/llvm/llvm-project/releases/download/llvmorg-21.1.8/${LLVM_RELEASE_NAME}.tar.xz")
set(LLVM_SHA256 "b3b7f2801d15d50736acea3c73982994d025b01c2f035b91ae3b49d1b575732b")
set(CAMADA_URL "https://github.com/mikhailramalho/camada/releases/download/v0.24/camada-linux-x64.tar.gz")
set(CAMADA_SHA256 "1640bf1dc576d0bd7106943a428de7bd667dd24d7db96f53be8b86207d3ed395")

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

if(NOT LLVM_DIR)
  foreach(major 21 22 23)
    if(EXISTS "/usr/lib/llvm-${major}/lib/cmake/llvm/LLVMConfig.cmake"
       AND EXISTS "/usr/lib/llvm-${major}/bin/clang")
      set(installed_llvm "/usr/lib/llvm-${major}")
      break()
    endif()
  endforeach()
endif()
if(LLVM_DIR)
  message(STATUS "LLVM_DIR given: no LLVM to download")
elseif(installed_llvm)
  message(STATUS "Using the installed LLVM in ${installed_llvm}")
  set(LLVM_DIR "${installed_llvm}/lib/cmake/llvm" CACHE PATH "LLVM package directory" FORCE)
  set(CMAKE_C_COMPILER "${installed_llvm}/bin/clang")
  set(CMAKE_CXX_COMPILER "${installed_llvm}/bin/clang++")
  # Its libraries are plain objects: any linker works, and lld links fastest.
  find_program(ld_lld NAMES ld.lld PATHS "${installed_llvm}/bin")
  if(ld_lld)
    set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=lld")
  endif()
else()
  download_dep(llvm "${LLVM_URL}" "${LLVM_SHA256}"
               "${LLVM_RELEASE_NAME}/lib/cmake/llvm/LLVMConfig.cmake")
  set(llvm_prefix "${DEPS_DIR}/llvm/${LLVM_RELEASE_NAME}")
  set(LLVM_DIR "${llvm_prefix}/lib/cmake/llvm" CACHE PATH "LLVM package directory" FORCE)
  set(CMAKE_C_COMPILER "${llvm_prefix}/bin/clang")
  set(CMAKE_CXX_COMPILER "${llvm_prefix}/bin/clang++")
  # Each link runs ThinLTO's code generation over every LLVM module the
  # binary uses; the cache keeps those objects between links, so a relink
  # after a change to Vereda regenerates only what changed (55 s to 10 s per
  # link on an 80-core machine, for a 63 MB cache).
  set(CMAKE_EXE_LINKER_FLAGS_INIT
      "-fuse-ld=lld -Wl,--thinlto-cache-dir=${CMAKE_BINARY_DIR}/thinlto-cache")
endif()

if(camada_DIR)
  message(STATUS "camada_DIR given: no camada to download")
else()
  download_dep(camada "${CAMADA_URL}" "${CAMADA_SHA256}"
               "lib/cmake/camada/camadaConfig.cmake")
  set(camada_DIR "${DEPS_DIR}/camada/lib/cmake/camada"
      CACHE PATH "camada package directory" FORCE)
endif()
