# Typed memory access alignment checks (#259)

## Design

Implements #259 via the opt-in `--ub-alignment-check` flag and `checks::Property::Alignment`
(`<fn>.alignment.<n>`), independent of `--no-assertions` and excluded under
`--property unreach-call` and `--property no-overflow`.

### Why IR instrumentation instead of `-fsanitize=alignment`

Clang's `-fsanitize=alignment` inserts `ptrtoint` on every typed dereference, which would
mark every dereferenced local object as address-exposed in Vereda's memory model, disabling
`ownsArrays` and `fieldLayout` and forcing address-layout constraints across all locals.

Instead, `checkAlignments` in `src/ir/load.cpp` runs before `mem2reg,sroa` and
`instcombine` (so SROA cannot erase misaligned stores through pointers to packed struct
members and InstCombine cannot raise alignment from UB assumptions). Direct accesses to
allocas whose own alignment already satisfies `align` are skipped so `mem2reg` still
promotes aligned locals. For every remaining `load`, `store`, `atomicrmw`, and `cmpxchg`
with `align > 1`, it inserts a call to `__vereda_check_alignment(ptr, i64 align)` whose
pointer parameter is marked `CaptureInfo::none()` and `MemoryEffects::none()`. Packed struct fields, `char*` accesses,
and `memcpy` carry `align 1` and are naturally skipped; pointer casts without dereferences
generate no memory instruction and are never flagged.

In `src/walker/walker.cpp`, `__vereda_check_alignment` checks `(addr & (align - 1)) == 0`.
When the pointer targets a single known object whose allocation alignment (`Memory::objectAlign`)
is already at least `align`, it tests only `mem.offset(p)` (folding constant offsets at walk
time without touching `mem.address(p)` or layout constraints); otherwise it tests `mem.address(p)`.

## Validation

- `tests/alignment_check.py` (`vereda/alignment_check`) covers aligned arrays, packed structs,
  `char*`, `memcpy`, casts without dereferences, misaligned constant and symbolic offsets,
  1-byte aligned locals cast to `int*`, 32-bit and 64-bit targets, `--no-assertions`,
  `--property unreach-call`/`no-overflow`, and raw LLVM IR (`align 1` vs `align 4`).
- Full `vereda` regression suite passes with zero failures.
