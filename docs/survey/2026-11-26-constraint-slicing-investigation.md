# Post-walk constraint slicing investigation (#251)

## Question

Does slicing globally asserted solver constraints after walking (before the
final non-incremental `s.check()`) meaningfully shrink formulas or speed up
solving in Vereda, the way SSA equation slicing does in ESBMC?

## Inventory of global constraints in Vereda

Unlike ESBMC, Vereda constructs expressions directly as a hash-consed DAG, so
unused scalar expressions, unused loads and unreferenced arrays already fall
outside the asserted check term. Only ten `addConstraint` call sites exist in
the codebase (plus `camada`'s internal `mkFPtoIEEEBV` axioms):

- `src/memory/memory.cpp`: address-layout ordering/alignment (`base#id`) and
  `bases[id] == base#id` / `sizes[id] == size` table rows.
- `src/encode/terms.cpp`: lambda-read beta-reduction equations
  (`select(lambda, idx) == reduce(lambda, idx)`).
- `src/walker/walker.cpp`: read-only write suppression (`!violated`) when
  pointer checks are not counted under a selected property.
- `src/solver/solver.cpp` and `src/driver/main.cpp`: the final check
  disjunction itself.

Lazy layout (#241) already defers `base#id` constraints until an object's
address is observed and materializes `bases`/`sizes` only on symbolic object
lookups.

## Measurements

We ran `--dump-smt-only` and computed the backward symbol-dependency fixed
point rooted at the final check assertion (excluding shared `__CAMADA_ieeebv_fn`
function names so independent FP-to-BV axioms are not artificially coupled)
across 837 passing tests:

1. **17 slowest passing/complex tests** (`esbmc/gcse`, `esbmc/github_302`,
   `esbmc/deep_binary_chain_pass`, `vereda/mem_store_chain_reads`,
   `vereda/lib_strtol`, `vereda/lib_strtok`, `esbmc-unix2/23_picosat-846_01`,
   `vereda/lib_ctype`, `vereda/lib_qsort`,
   `esbmc/array_loop_summary_decrement_fail` with and without summaries,
   `esbmc-cbmc/01_cbmc_Fixedbv11`, `esbmc-bitwuzla/native_fp_arith_fail`,
   `esbmc/tuple_const_array_struct_member`,
   `esbmc/tuple_const_array_member_reads_fail`, etc.):
   **0 of 17 tests** had any disconnected global constraint (`DROP = 0` on
   every test).
2. **All 502 eligible `tests/vereda/*.c` plus `--multi-property`,
   `array_loop_summary*`, and `float*` tests in `tests/esbmc/`:**
   351 tests (70%) emit only 1 `(assert ...)` total (the query itself).
   151 tests (30%) emit more than 1 `(assert ...)`.
   45 tests (9.0%) have at least one disconnected global constraint, with a
   maximum of 11 dropped constraints (`tests/vereda/mem_data_read_then_pointer_large.c`).
   Breakdown of all 156 dropped constraints: 124 `layout`, 16 `bases`,
   15 `fp_ieeebv`, 0 `lambda`, 0 `sizes`, 1 `other`.
3. **20% uniform sample of `tests/esbmc/*.c` (335 passing tests):**
   271 tests (81%) emit only 1 `(assert ...)`.
   64 tests (19%) emit more than 1 `(assert ...)`.
   32 tests (9.6%) have at least one disconnected global constraint, with a
   maximum of 21 dropped constraints (`tests/esbmc/vasprintf_valist_vacopy_launder_fail.c`).
   Breakdown of all 107 dropped constraints: 100 `layout`, 6 `bases`,
   1 `fp_ieeebv`, 0 `lambda`, 0 `sizes`, 0 `other`.

Across all 837 measured tests combined:
- **74.3%** (622/837) have zero global constraints beyond the query itself.
- **90.8%** (760/837) have zero sliceable global constraints.
- **9.2%** (77/837) have 1–21 disconnected constraints, almost exclusively
  small linear `base#` layout inequalities (85%) and `bases` array rows (8%)
  on tests that already solve in milliseconds, plus `camada`-internal
  `__CAMADA_ieeebv_fn` axioms (6%) that Vereda's `Terms::addConstraint` does
  not control.
- **0%** of dropped constraints were lambda-read equations or `sizes` rows.

## Why slicing does not help the current bottlenecks

- `esbmc/gcse` (50 asserts, 0 dropped) and `tuple_const_array_member_reads`
  (1 assert, 0 dropped) are bottlenecked by `store`/`select` chains **inside**
  the reachable query DAG, not by disconnected global constraints. Those are
  addressed by #272 (field-splitting struct arrays), #273/#276 (non-power-of-two
  table periods), and #279 (small-domain load folding).
- Chained `base#i > base#(i-1) + size` layout constraints pull in earlier
  objects whenever a later object's address is observed; however, lazy layout
  (#241) already keeps unobserved objects out of the chain, and Bitwuzla's
  non-incremental preprocessor handles small linear BV inequalities trivially.
- `fp_ieeebv` axioms are asserted directly inside `camada::SMTSolverImpl`
  during `mkFPtoIEEEBV`, so delayed assertion in `Terms` would not slice them
  without either changing `camada` or reconstructing the term graph in a fresh
  solver.

## Decision

Do **not** implement post-walk global constraint slicing or fresh-solver term
rebuilding at this time. The complexity (delayed assertion interacting with
mid-walk `Walk::unreachable()` queries, soundness separation between
guaranteed-SAT definitions and potentially-UNSAT semantic restrictions such as
symbolic-size `exists => layout` and read-only write suppression, model/witness
lifetime management, and SMT dump parity) is not justified when 91% of tests
and 100% of the slowest tests have zero sliceable global constraints.

Revisit only if future work (for example, k-induction invariants, large
multi-property workloads with many disjoint checks, or `camada` making
`mkFPtoIEEEBV` axioms explicit) introduces a measurable population of
expensive disconnected global constraints.
