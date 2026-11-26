# Rust panics outside core::panicking

Recorded 2026-11-25.

PR #215's review found that recognising only the panicking modules misses
Option and Result unwrap failures and slice-index failures. With rustc
1.100.0, LLVM 23.1.5, `-C opt-level=0 -C panic=abort`, their entry points
are bodyless calls to core::option::unwrap_failed,
core::result::unwrap_failed and core::slice::index::slice_index_fail.

The walker now treats a bodyless Rust core, alloc or std call as a panic
when it has noreturn or is immediately followed by unreachable. Known panic
entry points still count when their bodies are present. std::process::exit,
std::process::abort and core::intrinsics::abort end the path without a
violation. Bounds diagnostics also cover names containing index and fail,
and slice_error_fail. Other unknown Rust library calls that return retain
their existing behaviour.

Three bitcode fixtures compiled by rustc reproduce the reported cases; each
has its source and compiler command in an accompanying .ll header, following
rust_largest. All three give VERIFICATION FAILED, with a bounds property for
the slice. Handwritten IR checks the exit exclusions, the unreachable fallback,
and returning declarations and defined non-panic functions. The existing
dead-panic test is unchanged. The invoke diagnostic now describes unsupported
exception unwinding generally and qualifies panic=abort as advice for Rust.

Release build with LLVM 23.1.6: the Vereda suite passes (398 tests, one existing
lib_sprintf_overflow skip). The imported ESBMC suite was not run. The separate
review observation about C overflow property IDs is left for its follow-up.

Follow-up issues: #312.
