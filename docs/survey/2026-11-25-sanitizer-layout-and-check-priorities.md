# Sanitizer diagnostic layout and further checks

## github_302 regression

PR #257's performance job reports github_302 increasing from 3.56 to 5.69
seconds solving, and 3.64 to 5.77 seconds wall time. Locally, with LLVM 23.1.6,
camada 0.25 and Bitwuzla 0.11.0, the same source takes 7.047 seconds solving
on main, 37.692 seconds on the reviewed head, and 6.737 seconds on head with
`--no-div-by-zero-check`. Corresponding solver peaks are 252, 557 and 254 MB.

The integer divisors in this program are constant 1. Clang nevertheless emits
UBSan diagnostic objects for their checks. InstCombine removes the handler
calls, leaving their location/type globals unused. Initialising those globals'
pointer fields materialises two symbolic addresses and their layout constraints.
The checked floating-point expression is unchanged: the SMT difference consists
of two declarations and two unrelated allocation-layout assertions. This is
enough to perturb the difficult floating-point solve.

Reuse the existing entry-rooted module pruning after InstCombine and after
restoring constants marked by unconstantWritten. It removes the unused
diagnostic globals before memory initialisation. A separate CFG cleanup pass
was considered but is unnecessary: InstCombine has already removed the calls.
Live sanitizer handlers and their descriptors remain reachable and are retained.

With the fix, enabled and disabled divisor-check modes produce byte-identical
6,266-byte SMT formulas to main for the same input path. The reviewed head's
formula was 6,904 bytes. In the isolated comparison, main solves in 6.890 seconds
and fixed head in 6.791 seconds, both around 253 MB. Running bench.py with
separate base/head frontends while the suite is busy gives 7.93 to 7.86 seconds
solving, 8.07 to 8.00 seconds wall time and 253 to 251 MB, with no regression.

The regression test checks that a safe divisor leaves no handler, source global
or address-layout symbols in the prepared IR/SMT. Another case preserves a real
failure on a symbolic sibling branch. Restore the word-array dump fixture's
original RUN line: it now passes with default divisor checks and its original
object-number expectations, without a special opt-out.

Validation: all 7,032 registered tests complete with no failures (2,924 pass,
4,108 skip), including the expanded check-flags integration test and the restored
memory dump fixture. The full run takes 249 seconds locally.

## Further useful property families

Keep the agreed defaults and expand coverage deliberately. These are proposals,
not new command-line flags or implemented checks in this patch.

| Family | Priority and intended use |
|---|---|
| Object bounds, lifetime, valid free, alignment | Default pointer-safety coverage once implemented. Alignment is currently missing as well as general dereference/free checks. |
| Pointer arithmetic and subobject/field bounds | Further memory-safety coverage; preserve one-past pointers and legal bytewise object accesses. Requires more than whole-allocation bounds. |
| Uninitialised reads | High-value real-code check, initially opt-in. Needs path-local initialisation tracking for SSA values and memory, with correct treatment of padding and partial writes. |
| Floating-point to integer range | Useful opt-in arithmetic check, distinct from signed integer overflow. Include NaN and infinity cases. |
| Invalid builtin arguments | Useful opt-in check, notably zero for clz/ctz variants with undefined zero semantics. Defined-zero operations must remain valid. Encoding the underlying dynamic count intrinsics is also needed. |
| printf/scanf and library preconditions | Validate actual reads/writes and function contracts as pointer/API coverage. Respect formats such as %s, %n and %p rather than treating every pointer argument as a dereference. |
| restrict, stack/total-stack limits, allocation budgets | Useful later as explicit contracts or resource policies. restrict checks need access-based C semantics; restrict assumptions narrow the input domain. |
| Lock order and atomicity | Useful after the thread model, opt-in alongside race/deadlock checks. |

[Clang's sanitizer documentation](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
already provides instrumentation for alignment, float-cast-overflow and invalid
builtin arguments. These still need Vereda property selection, source-preserving
IR handling, correct classifications and tests; a parser option alone does not
implement them.

NaN, floating-point infinity/overflow policies and unsigned wrap should remain
opt-in numerical contracts. They are often intentional, especially in IEEE
floating-point code. Advisory dead-store, dead-code and unchecked-return analyses
can be useful later, with notes separate from safety failures and normal safety
checks retained. Reporting unreached claims is useful reporting, not a new
safety property.

Python instance checks do not fit the current C/Rust frontend. Volatile access
alone is valid C, so prioritise modelling volatile/environment semantics over
treating its presence as a defect. Likewise, unknown-function pointer arguments
need contracts or conservative modelling; blanket validity requirements would
reject opaque handles and permitted null arguments.
