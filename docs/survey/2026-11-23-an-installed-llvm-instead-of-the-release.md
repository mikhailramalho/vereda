# An installed LLVM instead of the downloaded release

Recorded 2026-11-23.

The pinned LLVM 21.1.8 release unpacks to about 11 GB per build directory, and
its static libraries are ThinLTO bitcode (`GLOBALVAL_SUMMARY` in each member),
so every link of `vereda` ran ThinLTO's code generation over every LLVM module
it uses: 54.7 s and 1179 s of CPU for a relink after a one-line change on an
80-core machine. #94's ThinLTO cache brought that to about 10 s, but each new
build directory still downloaded and unpacked the release, and CI restored it
from an 11 GB cache after clearing disk space for it.

Ubuntu's and apt.llvm.org's `llvm-21-dev` packages ship plain object files and
a shared `libLLVM-21.so`. With `DOWNLOAD_DEPENDENCIES=ON`, configure now uses
an LLVM 21-23 under `/usr/lib/llvm-*` when one is installed, with its clang,
since the models' bitcode has to match its version, and lld when present; it
downloads the release only when no LLVM is installed. Against Ubuntu's LLVM
21.1.2, a full build of `vereda` took 15.2 s and a relink after a one-line
change 9.3 s, and the whole suite gave the same results as the 21.1.8 build
(6640 tests: 2467 pass, 4173 skipped, no recorded verdict changed). The camada
release is still downloaded, 59 MB packed and 306 MB unpacked; downloads go to
`$VEREDA_DEPS_DIR` when it is set, so build directories and worktrees share
one copy.

GitHub's ubuntu-24.04 image ships LLVM 16, 17 and 18 only, so CI installs
`clang-21`, `lld-21`, `llvm-21-dev` and `libpolly-21-dev` (apt.llvm.org's
CMake package refers to Polly) from apt.llvm.org's noble repository. That
tracks the latest 21.1.x rather than the pinned 21.1.8; configure accepts 21
to 23. The release's LTO may make LLVM's own code a little faster, but
Vereda's time goes into the walk and the solver; the performance check would
show a difference.
