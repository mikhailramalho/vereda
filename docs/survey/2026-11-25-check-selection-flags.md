# Check selection flags and defaults

Assertions, the existing pointer checks and integer division/remainder by zero
are enabled by default. User assertions can be disabled with `--no-assertions`,
constant-object write checks with `--no-pointer-check`, and zero-divisor checks
with `--no-div-by-zero-check`. Disabling assertions leaves unwinding/recursion,
pointer checks, sanitizer checks and Rust's overflow/bounds panics independent.
The pointer flag does not promise general dereference or valid-free coverage;
those remain the lifetime/memory-safety work tracked by #29 and #30.

Signed overflow, unsigned overflow and shift instrumentation retain their
existing frontend opt-ins. `--memory-leak-check`, `--data-race-check` and
`--deadlock-check` are opt-in binary options that currently return error 6 with
an explicit not-implemented diagnostic, before input loading. ESBMC's
`--data-races-check` spelling aliases the race flag. They are deliberately not
classified as supported features by the importer until they perform checks.

An unreach-call property request conflicts with `--no-assertions` and is rejected
by the binary. Selected properties continue to count only their own failures.
An ignored invalid operation ends its path without asserting a global assumption
that could erase failures reached earlier. A disabled `__VERIFIER_assert` is
ignored; a real assert failure still ends execution through its nonreturning
failure function. No user-assertion switch disables completeness checks.

## Preserving zero-divisor checks

Checking integer division only in the walker is insufficient: mem2reg and
InstCombine can remove an unused division. IR preparation therefore inserts a
`__vereda_check_divisor` call before each signed/unsigned division/remainder
whose divisor is not already a nonzero constant, before those passes run.
The walker claims a dedicated division-by-zero property, then continues with
a path-local nonzero guard. This also works on raw LLVM IR. Linked models get
checks before linking. No checks are inserted with the disable flag.

Fixed vectors use scalar lane predicates combined with boolean and. An initial
vector-reduction implementation left vector reconstruction instructions after
scalarization and broke eight existing vector regressions; scalar lane extraction
keeps the checks compatible with the current scalarizer and walker.

Clang can also remove a literal C `1 / 0` before producing LLVM IR. In the default
property mode the frontend enables trap-mode integer-divide-by-zero instrumentation
for C and preprocessed C inputs, unless explicitly disabled. The binary still
validates property names; the frontend inspects the selected property only to
avoid adding irrelevant sanitizer failures to other property modes. Native IR
checks cover operations still present in those modes without reporting an
unrelated property. Floating-point division by zero is not an integer error.

## Tests and imported metadata

The focused check-flags test runs both the frontend and binary. It covers early
unsupported-feature errors, independent assertion/pointer selection, unused and
literal zero divisions, all four LLVM integer division/remainder operations,
vector lanes, solver and BMC modes, overflow/shift opt-ins, property conflicts,
earlier-failure preservation and independent unwinding assertions.

The importer now preserves `--no-assertions` and `--no-div-by-zero-check`. The
latter was previously dropped because Vereda had no such checks; restoring it
to imported configurations is necessary when changing the default. Existing
fixture source bodies and expectations stay intact. Cases whose recorded
verdict changes are reviewed and promoted only when every expectation holds.

Refreshed metadata in 269 imported fixtures, including promotions, without
changing source bodies or expectations. Five tests become CORE:
`char_view_through_pointer_fail`, `github_1172-1`, `github_1172-3`,
`github_4715_irep2_native_body_empty_stmt_target`, and `simplifier-modulus-fail`.
`github_4442_no_assertions_passthrough` still needs null-pointer checking and
`github_8681_fail` still needs array-bounds checking and detailed result rows;
their new verdicts are recorded without weakening expectations.

The LLVM 23.1.6 Release build passes the full CTest run with eight workers:
7,026 executions, 2,910 passes, 4,116 expected skips and zero failures in
279.70 seconds. This adds one focused integration test and recovers five
existing fixtures relative to main after #255. `git diff --check` passes too.
