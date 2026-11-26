# Follow-up issue audit of docs through 2026-11-26

Recorded 2026-11-26.

Audited `docs/plan.md`, `docs/architecture.md`, and all `docs/survey/*.md`
entries through `2026-11-26-runner-output-scans.md` against open and closed
GitHub issues. Previously untracked follow-up work, deferred ideas, and
remaining gaps were filed as GitHub issues #292 through #319, and each
referenced survey entry was annotated with its `Follow-up issues:` links:

- #292 — Support symbolic-size `memcpy`, `memmove` and `memset` (`2026-11-24-writes-into-read-only-objects.md`, `2026-11-25-c-standard-forwarding.md`)
- #293 — Copy `byval` struct arguments in direct calls (`2026-11-22-function-pointers.md`)
- #294 — Check pointer arithmetic and subobject/field bounds (`2026-11-25-sanitizer-layout-and-check-priorities.md`)
- #295 — Prevent InstCombine from deleting library-call writes into const objects and string literals (`2026-11-24-writes-into-read-only-objects.md`)
- #296 — Propagate path facts to memory/checks, switch cases, `x != c`, `x = y` and `zext` (`2026-11-24-assumed-ranges.md`, `2026-11-24-assumed-constants.md`)
- #297 — Avoid per-byte store chains for zeroed globals below `Memory::MinSeparate` (`2026-11-23-array-loop-summaries.md`)
- #298 — Encode missing LLVM bit and hint intrinsics (`llvm.ctpop`, `llvm.prefetch`, `llvm.bitreverse`) (`2026-11-21-milestone-0-survey.md`, `2026-11-23-clang-s-sanitizer-checks.md`, `2026-11-24-inline-asm.md`, `2026-11-24-inline-asm-after-variadics.md`)
- #299 — Support structs containing pointers in registers (e.g. multi-output inline asm) (`2026-11-24-inline-asm.md`, `2026-11-24-inline-asm-after-variadics.md`)
- #300 — Support remaining LDV inline asm idioms (`%gs:` per-cpu writes, paravirt calls, `btc`) (`2026-11-24-inline-asm.md`, `2026-11-24-inline-asm-after-variadics.md`)
- #301 — Support non-constant `frem` (`fmod`) and dynamic floating-point rounding modes (`fesetround`) (`2026-11-22-floating-point.md`, `2026-11-24-fold-float-constants.md`)
- #302 — Support C fixed-point arithmetic (`-ffixed-point`) (`2026-11-26-esbmc-regression-refresh.md`)
- #303 — Support atomics on floats and `i1`, and `cmpxchg` on pointers (`2026-11-22-atomics.md`)
- #304 — Support vectors of `i1` or pointers and vector-scalar `bitcast` (`2026-11-22-vectors.md`)
- #305 — Support variadic arguments of vector, `i128` and `long double` types (and `%Lf` in printf) (`2026-11-23-variadic-calls.md`, `2026-11-24-printf-to-memory-models.md`)
- #306 — Support computed goto (`indirectbr`) (`docs/plan.md`, `2026-11-24-fix-irreducible.md`)
- #307 — Support `atexit` handlers (`docs/plan.md`)
- #308 — Support non-64-bit targets (`-m32`) and Cargo projects in the Rust source driver (`2026-11-25-rust-source-driver.md`)
- #309 — Add umbrella `--ub-checks` flag to enable all opt-in UB checks
- #310 — Support per-loop unwind bounds (`--unwindset`, `--unwindsetname`) and `--no-standard-checks` (`2026-11-26-imported-option-cleanup.md`, `2026-11-26-imported-option-review.md`)
- #311 — Print floats, pointers, heap (`p->x`), bitfield and VLA stores in text counterexamples (`2026-11-22-floating-point.md`, `2026-11-23-stores-in-counterexamples.md`, `2026-11-24-counterexample-as-c.md`)
- #312 — Review and align C overflow property IDs (`2026-11-25-rust-panic-library-entries.md`)
- #313 — Print enabled check families near startup
- #314 — Report unreached claims (`2026-11-25-sanitizer-layout-and-check-priorities.md`)
- #315 — Do not fail `tests/bench.py` on `FUTURE` tests that error identically on base and head (`2026-11-24-terms-read.md`, `2026-11-24-lazy-constants.md`, `2026-11-24-term-handle.md`)
- #316 — Build release archive packaging for BenchExec and SV-COMP (`scripts/`) (`2026-11-23-benchexec-tool-info.md`, `docs/architecture.md`)
- #317 — Extend struct-field partitions to larger/non-power-of-two strides, pointer PHIs and heap structs (`2026-11-26-struct-field-partitions.md`)
- #318 — Evaluate `-z pack-relative-relocs` and `MIMALLOC_ALLOW_THP=0` for lower startup RSS (`2026-11-24-lld-startup-memory.md`)
- #319 — Track bodyless-call side effects through previously registered buffers and integer-encoded addresses (`2026-11-24-bodyless-call-stored-pointers.md`)
