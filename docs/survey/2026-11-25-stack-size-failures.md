# Reporting stack objects above PTRDIFF_MAX

Recorded 2026-11-25. Issue #222.

An oversized LLVM alloca previously added a global constraint excluding the
current guard. A constant oversized request therefore removed every execution,
including the failed assertion that should report it, and gave SUCCESSFUL.

The walker now checks the element count against
`PTRDIFF_MAX / element_size`, before narrowing the count or multiplying it.
This also handles element types larger than the target address space without
first truncating their byte sizes. Zero-sized elements and zero counts remain
valid; exactly PTRDIFF_MAX bytes is valid on both ILP32 and LP64.

The size check is an assertion under the current path guard. The block's guard
is then restricted to valid requests, and the object is allocated only on that
remaining guard. A constant invalid request stops the block. There is no global
assumption excluding the bad request: it would erase both the new check and
any earlier checks on that path. Allocations update the memory context after
restricting the guard.

Local ESBMC 8.3 reports an explicit constant oversized alloca with
`main.assertion.1` and `alloca: size exceeds PTRDIFF_MAX`; Vereda uses the same
kind and message. All LLVM stack allocations use that message, including VLAs,
which ESBMC sometimes calls `object size exceeds PTRDIFF_MAX`. Two imported
VLA expectations are adapted to the common LLVM diagnostic. Symbolic requests
are checked too, as #222 asks; ESBMC's current symbolic path assumes the cap.

`--no-vla-size-check` suppresses this check for LLVM stack allocations while
still ending invalid paths. The importer now preserves that flag, and four
tests whose upstream descriptions had it regain it. The two symbolic pointer
comparison tests explicitly use it to retain their original focus: pointer
ordering on allocations whose size fits. Under selected unreach-call or
no-overflow properties, a stack-size failure ends the path without counting
as a violation of that other property.

Five FUTURE tests are promoted: the two explicit-allocation cases named in
#222, the two constant-VLA cases, and github_1533, whose graph matrix can be
too large. github_60 now gives FAILED on the same matrix allocation but remains
FUTURE because its expected !overflow diagnostic is still absent. The old
huge-stack regression now expects the allocation failure; uninit_array_false
accounts for the new passing VLA-size assertion in its property numbering.
The solver-cache constraint regression uses two individually valid, forced
heap allocations whose combined layout is impossible, preserving its epoch
invalidation purpose without depending on the removed VLA assumption.

Validation: LLVM 23 Release build with rustc 1.100.0. All 17 focused tests pass:
constant and symbolic allocations, 32-bit multiplication and oversized element
types, exact boundaries, zero counts, an earlier failure surviving a later
invalid allocation, the disabled check and property selection. The full 6,995
test suite was run; its five compatibility/expectation failures were updated
and passed or skipped as expected on rerun. No failures remain. No additional
ReachSafety benchmark run was started; #12's existing workflow is separate.
