# Select and validate the frontend's LLVM toolchain

Issue #270 reports that clang on PATH can be older than the LLVM used by the
verifier. With default division-by-zero instrumentation this fails on the
LLVM 23 sanitizer flags before verification starts.

CMake now records LLVM_TOOLS_BINARY_DIR/clang in clang-path.txt next to the
binary, following the existing rustc discovery convention. Selection order is
explicit --clang, CLANG, CMake's recorded compiler, then clang on PATH. Check
the chosen compiler's reported major version against the verifier's --version
banner before compiling C or preprocessed C. An incompatible, unrecognised or
unrunnable compiler produces error code 6 with an override hint. Use the
banner already needed at startup rather than adding another binary option or
duplicating the LLVM version constant in Python.

Resolve the selected compiler through PATH and symlinks before locating the
sibling llvm-link. This prevents a compatible clang command on PATH from linking
through a different LLVM installation. Single LLVM IR inputs and Rust inputs
do not require a C compiler version probe. Preserve the banner as the first
line before compiler output.

Validation covers an old PATH compiler, CMake selection, environment and CLI
override precedence, missing compilers, matching PATH compiler/linker selection,
multi-file C verification and compiler-free single-IR verification. The expanded
frontend tests, banner tests and check-selection tests pass against the LLVM 23
verifier. A fresh CMake configure writes the expected LLVM 23 compiler path;
a default frontend invocation using that generated file verifies a C fixture.
No C++ engine changes are needed, so validation reuses the existing verifier.
