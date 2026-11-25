# Rust source input and a pinned compiler

Recorded 2026-11-25.

Rust source now follows the C path: vereda.py compiles .rs files to LLVM
bitcode, links multiple inputs with llvm-link, and runs the same verifier.
The three panic fixtures from PR #215 are source tests now, rather than
committed compiler output. Handwritten IR tests and the rust_largest
LLVM-version fixture remain.

CMake selects rustc 1.100.0, which emits LLVM 23 bitcode: an explicit
RUSTC_EXECUTABLE, or a matching rustc on PATH. With DOWNLOAD_DEPENDENCIES,
a missing or different system compiler falls back to the official compiler
and x86_64-unknown-linux-gnu standard-library archives. Both SHA-256 hashes
are pinned, and download_dep shares their extraction cache through
VEREDA_DEPS_DIR. The component installers write to a versioned dependency
directory, leaving rustup alone. Cargo and documentation are not downloaded.
With downloads off, the compiler must be installed. The selected path is
saved beside the binary; --rustc or RUSTC can override it at run time, with
the same version check. Verification itself never downloads a compiler.

Compilation uses edition 2024, opt-level 0, debug information, overflow
checks and panic=abort. Rust's generated C main calls std::rt::lang_start
with the source main as its first argument; the unavailable runtime would
otherwise hide the user's program behind a bodyless lang_start_internal.
IR preparation replaces that startup call with a direct call to source
main and a zero startup result. Rust runtime startup is not verified.
Programs with no_main and a C entry point still work unchanged.

The initial source path supports the 64-bit host. It rejects -m32, C
sanitizer/property-file flags and -I/-D rather than silently ignoring them.
Cargo dependency resolution and other targets are outside this change.

Validation used both an installed rustc and the actual downloaded toolchain.
The fallback downloaded and verified both archives, installed a working
compiler and standard library, and reconfigured without another download.
An unusable explicit compiler failed configuration. All 401 Vereda tests
passed with the downloaded compiler, with the existing lib_sprintf_overflow
skip; the subsequently added mixed C/Rust regression passed separately.
The 23 focused Rust tests passed with the installed compiler. Direct checks
also covered keep-ir, mixed linking, wrong compiler versions, compilation
errors and the unsupported 32-bit target, including error exit code 6.
The imported ESBMC suite was not rerun.
