# Resume incremental BMC through callees and recursion

Recorded 2026-11-26.

## Background

#320 extended #175's checkpoints to nested loops in the entry function. A loop
inside a callee or a recursion cut still forced a walk from scratch, because a
loop frame alone could not reconstruct its suspended callers. This entry extends
the existing continuation rather than changing bound growth or adding flags.

## Design

Each active defined call exposes its caller frame and the state `enter` restores
on return: the call instruction, pre-call memory, outer facts, saved recursive
bindings and outer `reach_error` location. The live records point to the C++ walk's
locals; a checkpoint copies them into owned caller records. Ordinary walks do not
build this active-call list or copy pre-call memory.

A general checkpoint captures the active function stack, variadic contexts,
current `reach_error` location and encoder bindings, in addition to the existing
memory arrivals, returns, enclosing-loop counters, checks, trace and claim maps.
It can now stop either at a loop header or immediately before a depth-limited
recursive call. Synthetic array-summary iterations remain excluded.

On the next bound, restore the checkpoint and finish the interrupted function.
Then consume saved callers from the inside out: join the callee's returns, restore
the caller bindings, bind the call result, execute the instructions after the call,
and finish the block's enclosing regions and iterations. The ordinary call path
and resumed path share `leave`, so return guards, memory joins, facts and recursive
bindings use the same implementation. The saved prefix does not need a matching
native C++ call stack. New cuts encountered during resumption capture the current
combination of saved callers and newly entered functions.

General checkpoints still roll back the entire suffix's check mutations, trace
and claim numbering before extension. Fresh-symbol counters are not rewound. The
immediate entry-loop fast path remains available; callees use general checkpoints
so their other returns and caller suffixes are explored at the current bound.

The existing invalidation guard is unchanged: suffix allocations or added global
constraints discard the checkpoint. Restoring only path-local memory would not
undo those effects safely. This remaining work is tracked in #326.

## Measurements

Base `bfb20885` includes #320 and the documentation instructions. Both binaries
were built with Clang 23.1.6, RelWithDebInfo, mimalloc 3.7.0 and camada 0.25 /
Bitwuzla 0.11.0 in the same build directory. Three serial trials alternate
base/head order, with no compilation or test suite running concurrently. Times
include the Python front end and C compilation. Memory is the maximum reported
walk/solve peak per process; the table gives medians.

| Fixture/configuration | Base wall | Head wall | Base peak | Head peak | Bounds | Head resumes |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `incremental_resume_call_chain`, SIZE=2000 | 6.354 s | 0.259 s | 97 MB | 38 MB | 2001 | 2000 |
| `incremental_resume_recursive_prefix`, SIZE=200, WORK=128 | 3.364 s | 0.200 s | 44 MB | 52 MB | 81 | 80 |
| Same recursive fixture, SIZE=200, WORK=0 | 0.343 s | 0.161 s | 38 MB | 52 MB | 201 | 200 |

All runs prove the same assertions and use the same bound sequences; the base
does not resume in any of these cases. The call chain improves about 24.6 times,
and recursive prefix work about 16.8 times. The plain recursive case also improves,
but saving its caller chain costs additional memory. The WORK=128 loop's known
trip count skips intermediate bounds before recursion becomes the limiting cut.

The three base/head wall samples for the call chain are
6.354/6.347/6.355 s and 0.260/0.257/0.259 s. For recursive prefix work they are
3.364/3.362/3.377 s and 0.199/0.200/0.201 s. These are controlled regressions,
not a whole-ReachSafety speedup claim.

Reproduce the two larger configurations with each binary:

```sh
python3 vereda.py --bin build/vereda tests/vereda/incremental_resume_call_chain.c \
  --no-summarize-array-loops --incremental-bmc --unlimited-k-steps -DSIZE=2000
python3 vereda.py --bin build/vereda tests/vereda/incremental_resume_recursive_prefix.c \
  --no-summarize-array-loops --incremental-bmc --unlimited-k-steps -DSIZE=200 -DWORK=128
```

Both larger configurations have explicit `RUN` lines, allowing performance CI to
compare the head's fixtures against the base binary.

## Validation

The complete Vereda suite runs 602 configurations: 601 pass and the existing
`lib_sprintf_overflow` test remains skipped. All 279 imported incremental fixtures
complete without failures, with 68 passing and 211 skipped. The suite includes
47 incremental configurations after this change.

New regressions cover multiple callers, returning into the middle of a block and
an enclosing loop, symbolic callee exits, early returns, sibling paths, caller
memory, uninitialised-read checks, recursive return expressions, mutual recursion,
recursive calls within loops, and variadic contexts on 32- and 64-bit targets.
The old callee-cut regression now requires resumption and still finds its bug.
Allocation in a caller suffix still requires the safe fallback.

The recursive failure witness and the witness with a cut inside `reach_error`
match the base's complete counterexample/witness output after normalizing only
the generated UUID and creation time. Their source property IDs remain stable.

## Remaining work

Suffix object/constraint rollback is tracked in #326. Checkpointing still copies
the active recursive caller chain: O(depth) snapshot work can accumulate
quadratically while extending recursion one level at a time. Profiling and
sharing immutable prefixes are tracked in #327, including the observed memory
tradeoff. The longer ReachSafety gate remains in #175, with cgroup-based local
resource accounting in #173. These gaps keep #175 open.

Follow-up issues: #175, #173, #326, #327.
