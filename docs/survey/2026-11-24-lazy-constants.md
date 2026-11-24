# Bit-vector constants made in the solver only when needed

Recorded 2026-11-24.

After #168, `Terms::bv` was still about 31% of the walk's samples on
array_loop_summary_decrement_fail with `--no-summarize-array-loops`: 2.7
million calls, 707,000 constants built, and of that time about 43% Bitwuzla
making the value node and its C API's term table, 23% the `bvs` cache (the
`DenseMap<APInt, Ref>`) and 20% inserts into `values`, the map from a term
to its constant value (`2026-11-24-constants-without-strings.md`). Most of
those constants fold away and never reach the solver (#171).

## Three ways to keep a constant out of the solver

`Terms::Ref` is `camada::SMTExprRef`, a handle to a term in the solver's
arena, and every stage spells it so.

(a) A handle of Terms' own, holding either a value or a solver term. Every
stage's `camada::SMTExprRef` (605 lines in 13 files) would become that
type, which would also need the width and sort of a constant without a
solver term, and every call into camada a conversion. The compiler would
then find each place a constant reaches camada.

(b) Keep `SMTExprRef`, and make a constant a term of Terms' own that camada
sees only when it needs one. camada allows it: `SMTSolverImpl::makeExprRef`,
public in its installed `core/camadaimpl.h`, puts any subclass of `SMTExpr`
in the solver's arena and gives back a live handle, as camada's own tuple
and Ackermann array terms are made. What camada does not allow is being
handed such a term: each backend casts every term it is given to its own
type, with no check in a release build, so a constant that reaches a camada
call is undefined behaviour.

(c) Make the solver's node lazily, behind the existing `constant()` lookups.
With `Ref` an `SMTExprRef`, `bv()` must return a live handle, which only the
solver's arena can make; the handle has to point at something that holds
the value and, later, the node. That is (b)'s term, so (b) and (c) are one
design.

(b) is the change: three files, all in `encode`.

## The change

`bv()` now builds a `Const`, a subclass of `SMTExpr` local to terms.cpp,
in the solver's arena: its kind is `BVConst`, its sort the bit-vector sort,
its backend kind a value no backend has, and it holds the `APInt` and,
once made, the solver's term for it. `constant()` reads the value from the
`Const` (a kind check, then a virtual call) instead of looking the term up
in `values`, which is gone. Bool constants stay solver terms, both made when
`Terms` is, so `isTrue` and `isFalse` compare pointers.

`solverTerm(e)` gives the solver's term for a constant, making it the first
time as `bv()` used to, and any other term as it is. Terms calls it on the
operands of every operation that does not fold (`build`) and on a constant
array's value. Outside Terms, only the encoder hands camada a bit-vector
term directly: the bits of a float (`mkBVToIEEEFP`) and an integer
converted to a float (`mkSBVToFP`, `mkUBVToFP`). Everything else camada is
handed is a Bool (assertions, assumption literals, checks, the dumped
formula), a float or an array, which are never a `Const`, and the model
queries (`Model::value`, the walker's `single`) ask `constant()` first.

One call goes the other way. camada's `mkIEEEFPToBV` gives back the bits a
native float was made from (its `IEEEBVShadow`): before, that was the
constant itself, so the bits of a NaN constant, or of a float an asserted
equality ties to a constant, folded. It is now the solver's term for the
constant, which `fromSolver` turns back into the constant through the map
`made`, filled as terms are made.

The rule the compiler does not check: new code that hands camada a
bit-vector term that may be a constant passes it through `solverTerm`.

`makeExprRef` asserts on a field of the sort that camada's headers declare
only without `NDEBUG`. The prebuilt camada is built with `NDEBUG`, so its
sorts lack the field, and a Debug build of Vereda read a byte they do not
have and failed the assertion on its first constant. terms.cpp includes
`core/camadaimpl.h` with `NDEBUG` defined; the Debug build then passes the
vereda suite and the z3, cvc5, dump and float tests.

On the benchmark, 2,700,037 calls of `bv()` build 708,026 constants, and
200,008 of them (28%) get a solver term.

## Exactness

Every single-file test under `tests/` (6,581) ran with main (5d351dc0) and
the branch, with `--dump-smt-too --dump-dir` added (or the test's own dump
flag) and 60 s each. 379 were skipped, as they need extra files, their own
directory or an `__ESBMC_`/`__CPROVER_` builtin; 5 took longer
(github_123, github_302, github_9356, github_9356_success,
struct_array_ite_new_index; see below). Of the other 6,197, 6,017 gave the
same output and the same dumps, byte for byte once the binary's path and
temporary directories are left out, and 10 more differed only in the
witness's UUID.

146 dumps (79 from Z3, the rest from Bitwuzla) differ only in the order of
assertions, the names of shared terms (`@def`, `let`) and the order of the
operands of commutative operations, which Bitwuzla sorts by node id: ids
follow creation, and a constant's node is now made later. A script that
reads each dump, inlines the shared terms, sorts the operands of `and`,
`or`, `=`, `bvadd` and the like, and compares the assertions as multisets
found all 146 the same (it does tell apart a dump with one constant
changed). In github_9222_formula, which prints its formula, the same holds.

No verdict changed. 23 counterexamples show other values: the solver picks
another model of the same formula, as the formula's nodes come in another
order.

## Time and memory

Release builds with the installed LLVM 21.1.2 and camada 0.24, both with
mimalloc (`-DDOWNLOAD_DEPENDENCIES=ON`), main (5d351dc0) and the branch, on
an 80-core machine under a load of 10 to 16. Three runs of each, alternated,
on array_loop_summary_decrement_fail with `--no-summarize-array-loops`:

| build | walk s | walk peak MB | solve s | peak RSS MB | wall s |
|---|---|---|---|---|---|
| main | 2.32, 2.24, 2.23 | 492 | 4.05, 4.18, 4.06 | 935 to 941 | 7.12, 7.18, 7.05 |
| branch | 1.62, 1.56, 1.57 | 437 to 438 | 3.57, 3.56, 3.65 | 883 to 888 | 5.94, 5.75, 5.87 |

The walk takes 30% less time and 54 MB less memory. Counted by perf, the
walk plus `--dump-smt-only` takes 22% fewer user instructions (6.25 to 4.89
billion) and 39% fewer user cycles (5.68 to 3.45 billion); a whole run 6%
fewer instructions (23.2 to 21.9 billion) and 22% fewer cycles (14.5 to
11.3 billion). The instructions saved are all the walk's: the solve runs as
many as before, on the same formula, but in about 11% fewer cycles, with
508,000 fewer value nodes in Bitwuzla's tables.

`tests/bench.py --list` on the tests `2026-11-24-terms-read.md` measured
(12_lms_new, 13_ludcmp_new, github_302, github_1289 and every
array_loop_summary test, each also as a copy with
`--no-summarize-array-loops` added), the slow tests above and
struct_array_ite_new_index_fail, the two binaries side by side, 180 s
each. The rows that changed or ran long (walk, solve and wall seconds,
peak MB, verdict):

| test | walk s | solve s | wall s | peak MB | verdict |
|---|---|---|---|---|---|
| array_loop_summary, no summaries | 9.31 → 6.84 | - | - | 1213 → 1029 | TIMEOUT |
| array_loop_summary_fail, no summaries | 9.47 → 7.37 | - | - → 183.67 | 1202 → 1018 | TIMEOUT → FAILED |
| array_loop_summary_nondet, no summaries | 19.27 → 13.67 | - | - | 1999 → 1766 | TIMEOUT |
| array_loop_summary_nondet_fail, no summaries | 11.47 → 9.09 | - | - | 1363 → 1197 | TIMEOUT |
| array_loop_summary_counter, no summaries | 3.32 → 2.17 | - | - | 508 → 453 | TIMEOUT |
| array_loop_summary_counter_fail, no summaries | 2.41 → 1.62 | 1.00 → 0.59 | 4.39 → 3.00 | 580 → 516 | FAILED |
| array_loop_summary_decrement, no summaries | 2.38 → 1.80 | - | 32.06 → 31.99 | 497 → 450 | KILLED |
| array_loop_summary_decrement_fail, no summaries | 2.44 → 1.67 | 4.19 → 3.56 | 7.55 → 5.99 | 950 → 898 | FAILED |
| array_loop_summary_store, no summaries | 3.36 → 2.70 | - | 49.15 → 50.07 | 566 → 546 | KILLED |
| array_loop_summary_store_fail, no summaries | 3.20 → 2.66 | - | 49.71 → 49.76 | 566 → 545 | KILLED |
| array_loop_summary_stride, no summaries | 0.91 → 0.60 | - | 44.93 → 45.78 | 216 → 178 | KILLED |
| array_loop_summary_stride_fail, no summaries | 0.87 → 0.56 | - | 45.64 → 45.16 | 217 → 178 | KILLED |
| array_loop_summary_vla, no summaries | 0.47 → 0.48 | 0.69 → 0.71 | 1.28 → 1.32 | 85 → 86 | SUCCESSFUL |
| github_302 | 40.18 → 37.21 | 17.08 → 7.54 | 57.43 → 44.89 | 752 → 563 | FAILED |
| github_302, no summaries | 38.94 → 37.35 | 17.30 → 7.42 | 56.39 → 44.92 | 763 → 564 | FAILED |
| struct_array_ite_new_index | 0.81 → 0.61 | - | - | 161 → 143 | TIMEOUT |
| struct_array_ite_new_index_fail | 0.83 → 0.60 | 16.66 → 15.75 | 17.82 → 16.66 | 771 → 768 | FAILED |

The other rows, the tests with summaries among them, take under 0.3 s with
both. github_123, github_9356, github_9356_success and the two stride_bound
copies without summaries time out with both. Main did not finish
array_loop_summary_fail without summaries in 180 s; the branch finished it
in 184 s, as bench.py gives the head ten times the base's time once the
base times out. github_302's solve, on the same formula up to order, takes
less than half the time, and its peak memory falls by 190 MB. bench.py
lists as regressed the fifteen FUTURE tests (k_induction, uninit,
verifier_assert, witness and their copies) that end with the same error
with both binaries, as in `2026-11-24-terms-read.md`.

With 900 s, the dumps of github_123 and github_302 with the branch are
main's up to order and names, with the same verdicts (github_302's
counterexample shows other values); github_9356 and github_9356_success do
not finish with either, and struct_array_ite_new_index gives the same
output and dumps.

## What is left of `Terms::bv`

perf with LBR call stacks, two runs of each binary on the decrement test
with `--dump-smt-only` (printing the formula is 8 to 11% of each run's
samples), under a load of about 17. Shares of each run's
cycles, with what the function calls:

| | main | branch |
|---|---|---|
| cycles of the run (billions) | 6.09, 6.96 | 4.61, 4.72 |
| `Terms::bv` | 31.9%, 30.2% | 17.5%, 17.6% |
| in it: Bitwuzla's value node (`mkBVFromDec`) | 11.5%, 10.6% | - |
| in it: the `bvs` lookup, rehashing included | 9.6%, 9.4% | 14.7%, 14.9% |
| in it: the arena's new blocks, zero-filled | - | 2.0%, 1.9% |
| `values`: inserts (in `bv`) and `constant()`'s lookups | 6.4%, 6.1% | - |
| `solverTerm`, for the 28% that reach the solver | - | 5.4%, 6.2% |
| in it: `mkBVFromDec` | - | 4.7%, 4.9% |
| `constant()` itself | 0.2%, 0.05% | 1.7%, 1.9% |

In cycles, `bv` and `solverTerm` together fall from 1.9 to 2.1 billion to
1.1 billion. What is left of `bv` is mostly the `bvs` lookup, which takes
about as many cycles as on main (0.59 to 0.65 billion on main, 0.68 to 0.70
on the branch).

## Not done

The `bvs` lookups are now most of what is left, and they mostly wait on
memory: 708,026 entries of 40 bytes (a 16-byte `APInt` and a 24-byte
handle) in a table of a million buckets, about 40 MB, and each new constant
probes a bucket no recent lookup touched. camada built with
`CAMADA_CHECKED_HANDLES=OFF` has 8-byte handles, which would shrink the
table by 40%. Not hash-consing constants would save the lookups, but every
map keyed on a constant's pointer (`memo`, `selects`, `reduced`) would then
have to compare values, and the identity folds (`a == b` by pointer) would
miss equal constants.

camada's arena zero-fills each block it adds (`make_unique<std::byte[]>`),
about 2% of the samples here; a block left uninitialised would do, but that
is camada's code.

(a) above would have the compiler check what the rule in terms.h asks of
new code. It means changing the type on 605 lines in 13 files, the 32 maps
keyed on `const camada::SMTExpr *`, and every handle growing from 24 to 32
bytes (a solver handle, or a pointer to a constant), which costs memory in
every map and cell that holds one.
