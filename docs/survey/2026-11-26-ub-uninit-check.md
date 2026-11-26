# Uninitialised local and memory read checks (#260)

## Design

Implements #260 via the opt-in `--ub-uninit-check` flag and `checks::Property::UninitialisedRead`
(`<fn>.uninitialised-read.<n>`), independent of `--no-assertions` and excluded under
`--property unreach-call` and `--property no-overflow`. When disabled (the default), no
initialisation state is created or merged, preserving the existing nondeterministic model
and `#9` soundness behavior with zero overhead.

### IR preservation

When `uninitCheck` is active, `nondetLocals` in `src/ir/load.cpp` marks every local `alloca`
not unconditionally written before read with `__vereda_uninitialised(a)` instead of storing
a `__VERIFIER_nondet_uninitialised` value. This prevents `mem2reg` and `InstCombine` from
erasing uninitialised local reads or folding them to `undef`, while still letting provably
written-before-read scalar locals promote to SSA.

### Path-sensitive byte-level tracking in `Memory`

When `Memory::trackInit` is true, `Memory::State` tracks byte initialisation alongside data:

- `init`: `Array(ptr -> Bool)` defaulting to `constArray(true)` (globals and external memory).
- `objInit`: per-object default `Bool` (`false` for fresh `alloca` and `malloc` in $O(1)$
  regardless of size; `true` for `calloc`, whole-object `memset`, and `havocObject`).
- `knownInit`: per-`(obj, offset)` `Bool` overrides for constant-offset stores and copies.
- `initFlushed`: marks objects whose `objInit`/`knownInit` have been flushed into `init`
  (using a `Terms::lambda` for non-true `objInit` so large/symbolic allocations flush in $O(1)$).

Operations maintain byte-accurate initialisation:
- `store`, `set` (`memset`), `fill`, `havoc`: mark written bytes initialised.
- `copy` (`memcpy`/`memmove`): propagates per-byte `init` bits (handling overlapping ranges),
  so copying a partially initialised struct or padding bytes never triggers a false positive
  unless an uninitialised byte is actually loaded.
- `realloc` (in `models/stdlib.c`): allocates uninitialised memory and copies initialised
  prefix bytes, leaving growth bytes uninitialised.
- `Memory::ite`: merges `init`, `objInit`, `knownInit`, and `initFlushed` path-sensitively.
- `LoadInst` in `Walk::memoryAccess`: checks `mem.isInitialised(st, p, storeSize)`,
  reports `Property::UninitialisedRead`, and narrows the path guard.

## Validation

- `tests/uninit_check.py` (`vereda/uninit_check`) covers uninitialised locals, conditional
  initialisation (guarded and unguarded), partial struct writes and `memcpy` with padding,
  heap `malloc`/`calloc`/`realloc`, genuinely nondeterministic inputs and globals,
  `--no-assertions`, `--property unreach-call`/`no-overflow`, and disabled-mode behavior.
- Full `vereda` suite (including `#9` tests `uninit_local*`, `uninit_array*`) passes with
  zero failures.
