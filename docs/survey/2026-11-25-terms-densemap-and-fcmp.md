# Compact Terms tables and fcmp without redundant NaN checks

Recorded 2026-11-25.

Two encoding-stage changes on `origin/main` (`afc20c02`): replacing `Terms`'
node-allocated hash tables with pointer-stable deque storage plus open-addressed
tables (`llvm::DenseMap` / `DenseSet` and a compact index table), and dropping
redundant `mkFPIsNaN` terms from `Encoder::fcmp`.

## Terms storage, tables and fast paths

`Terms` previously stored built nodes in `std::unordered_map<const void *, Node>`
and hash-consed them in `std::unordered_map<Key, Ref, KeyHash>`, duplicating the
40-byte key/node payload across two separately allocated hash tables and paying
per-entry heap allocation on every node. Recursive folding holds `const Node *`
pointers across insertions, which had been the reason `nodes` stayed node-based.

Built nodes now live in `std::deque<Entry>` (`struct Entry { Node n; Ref self; }`),
which is pointer-stable across `push_back`. `nodes` becomes
`llvm::DenseMap<const void *, uint32_t>` mapping a term id to its deque index.
`memo` becomes a compact open-addressed vector of `{uint32_t idx; uint32_t hash;}`
buckets (8 bytes per bucket, 2/3 max load factor) indexing `entries` directly,
avoiding `Key` duplication and avoiding dereferencing `entries` during probes
unless the 32-bit hash matches. Folded `concat` joins append an `Entry` to
`entries` and index it in `memo` without adding it to `nodes`, preserving the
previous semantics.

The walk-only caches `implied`, `arith`, `filled`, `reduced`, `selects`, and
`bounds` move from `std::unordered_map` to `llvm::DenseMap`. In `select`, the
cache lookup does find-then-insert instead of holding a `Ref &` across recursive
`uncached` calls. In `substitute`, `freeOf` maps each symbol id to a
`std::unique_ptr<llvm::DenseSet<const void *>>` so the per-symbol set reference
remains stable if a recursive lambda reduction calls `substitute` for another
symbol, and the local `done` map and `stack` use `llvm::DenseMap` and
`llvm::SmallVector<Ref, 32>`. `Terms::releaseWalkTables()` swaps `entries`,
`memoBuckets`, and all walk tables with empty containers so walk memory is
released before solving.

Small hot-path overheads were trimmed at the same time: `Terms::node` and
`Terms::arithmetic` (along with the inner node probes in `conjunct` and
`arithmetic`) return immediately on null or constant terms (`e.entry()`) without
probing `nodes`; `Terms::arithmetic` uses `llvm::SmallVector<const void *, 32>`;
`Terms::eq` checks `a == b` and constant-constant folding before calling `lift`;
`Terms::lift` is now a private template instead of taking `std::function`; and
`make` / `uncached` skip `lambdas` and `under` lookups when `lambdas` is empty.

## Encoder::fcmp without redundant NaN checks

`Encoder::fcmp` previously built `mkFPIsNaN(a) || mkFPIsNaN(b)` on every
non-constant floating-point comparison and conjoined or disjoined it with the
comparison result. In SMT-LIB's floating-point theory (and in camada's
`--fp2bv` encoding), `fp.eq`, `fp.lt`, `fp.leq`, `fp.gt`, and `fp.geq` already
evaluate to `false` when either operand is NaN.

`OEQ`, `OGT`, `OGE`, `OLT`, and `OLE` now emit `mkFPEqual`, `mkFPGt`, `mkFPGe`,
`mkFPLt`, and `mkFPLe` directly. `UNE`, `UGT`, `UGE`, `ULT`, and `ULE` emit the
logical negation of `mkFPEqual`, `mkFPLe`, `mkFPLt`, `mkFPGe`, and `mkFPGt`.
Explicit `mkFPIsNaN` terms are emitted only for `UNO`, `ORD`, `ONE`, and `UEQ`.

Two regression tests (`tests/vereda/float_fcmp_predicates.ll` and
`tests/vereda/float_fcmp_predicates_fp2bv.ll`) exercise all 14 non-trivial
`fcmp` predicates on symbolic `+0.0`, `-0.0`, `NaN`, `+Inf`, `-Inf`, and finite
values under both native floating-point theory and `--fp2bv`.

## Verification and measurements

Full test suite (`ctest --test-dir build -j10 --output-on-failure`):
6,937/6,937 passed (0 failures, 168.73 s wall).

Side-by-side `tests/bench.py` comparison (`RelWithDebInfo`, LLVM 23.1.6,
mimalloc) of `afc20c02` (`build/vereda-base`) against this branch
(`build/vereda`):

| test | walk s | solve s | wall s | peak MB | verdict |
|---|---|---|---|---|---|
| `esbmc/github_302` | 0.00 → 0.00 | 26.43 → 17.57 | 26.57 → 17.69 | 540 → 547 | FAILED |
| `esbmc-unix2/23_picosat-846_01` | 0.50 → 0.45 | 13.84 → 13.38 | 14.88 → 14.35 | 379 → 362 | FAILED |
| `esbmc/struct_array_ite_new_index` | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 38 → 38 | SUCCESSFUL |
| `esbmc/struct_array_ite_new_index_fail` | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 39 → 39 | FAILED |

All `vereda/float_*` tests matched verdicts and timing with zero regressions.
With `--no-summarize-array-loops` to stress `Terms` construction and hashing:

| test (`--no-summarize-array-loops`) | walk s | solve s | wall s | peak MB | verdict |
|---|---|---|---|---|---|
| `esbmc/array_loop_summary` | 1.89 → 1.81 | 0.00 → 0.00 | 2.11 → 2.04 | 242 → 241 | SUCCESSFUL |
| `esbmc/array_loop_summary_fail` | 1.85 → 1.83 | 0.00 → 0.00 | 2.72 → 2.73 | 242 → 242 | FAILED |
| `esbmc/array_loop_summary_nondet` | 5.17 → 4.23 | 0.00 → 0.00 | 5.55 → 4.52 | 532 → 517 | SUCCESSFUL |
| `esbmc/array_loop_summary_counter_fail` | 1.10 → 1.04 | 0.57 → 0.55 | 2.55 → 2.48 | 281 → 284 | FAILED |
| `esbmc/array_loop_summary_decrement_fail` | 1.11 → 1.04 | 3.51 → 3.45 | 5.46 → 5.30 | 625 → 632 | FAILED |

`esbmc/github_302` improved by 33% in solve and wall time (26.57 s → 17.69 s)
from dropping redundant `mkFPIsNaN` terms in `fcmp`. On walk-heavy workloads,
`array_loop_summary_nondet` without summaries dropped 18% in walk time (5.17 s
→ 4.23 s) and 15 MB peak RSS, and `23_picosat-846_01` dropped 10% in walk time
(0.50 s → 0.45 s) and 17 MB peak RSS.
