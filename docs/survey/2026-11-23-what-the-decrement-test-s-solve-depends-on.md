# What the decrement test's solve depends on

Recorded 2026-11-23.

Issue #99. `array_loop_summary_decrement_fail` writes `a[0]` to `a[99999]`
in that order, then asserts `a[x] == 7 - 2*x` for a nondet `x < 100000`.
Every `x` breaks the assertion. Vereda's solve took about 27 s with
Bitwuzla and ESBMC's about 2 s.

Solvers. ESBMC 8.7.0 builds Bitwuzla 0.9.0 with the CaDiCaL 2.1.2 it
bundles. camada v0.24, which Vereda downloads, builds Bitwuzla 0.11.0 from
source with the CaDiCaL 3.0.1 it bundles. Both are release builds. camada's
Z3 is 5.4.1 and ESBMC's is 4.13.3. Bitwuzla 0.9 ran as its command-line
tool; 0.11 ran through a small program that hands the file to the parser
of the library camada links. The machine has 80 cores and was running
other jobs (load 8 to 40). Runs went in parallel batches under `nice`, so
compare numbers within a table, not across tables.

## The formulas

Dumped from the two tools:

| Name | What it is |
|---|---|
| vs | Vereda's `--dump-smt-only` from before #88: 400000 byte stores on the shared array at a 96-bit (object, offset) index, four assertions on base addresses, then `(push 1)`, the property asserted, `(check-sat)` |
| P | vs without the four base-address assertions; the push stays |
| Q | P without the push: the property asserted at level 0 |
| R | Q with the property as a Boolean `viol` under `check-sat-assuming`, which is how camada's `checkSatAssuming` poses it |
| H, H-assert, H-assume | the dump at a0a466f6, after #88, posed as vs, Q and R (base assertions kept): `a` has an array of its own at a 64-bit offset index, with the same 400000 stores |
| es | ESBMC's `--smt-formula-only --no-summarize-array-loops`: 100000 stores of 32-bit elements at a 17-bit index, one select, everything asserted at level 0 |

Rebuilt from vs's 400000 (index, byte) pairs, with a nondet `X`, the
property asserted and no base assertions:

| Name | Shape |
|---|---|
| A2 | Vereda's: bytes, 96-bit index, four selects; Q up to names |
| A | A2 with literals written `(_ bvN w)` and each index as `(concat (_ bv1 32) (_ bvN 64))` |
| B, C, G | bytes at a 64-bit (H's width), 19-bit and 32-bit index |
| D | ESBMC's: 32-bit words at a 17-bit index, one select |
| E, F | words at the byte offset with a 96-bit index; words at a 32-bit index |
| A2rev, Drev | A2 and D with the stores in reverse order, so that `a[0]` is the outermost store |

## What costs

Bitwuzla first treats `select(A, X)` as a free value and finds a model.
Then it checks the read against the array. If the read is wrong, it adds a
lemma for that value of `X`, which follows the chain from the outermost
store down to the store at `X` and bit-blasts a comparison of `X` with the
index of every store it passes. Any model whose read is right ends the
search, since every `x` breaks the assertion. So the cost depends on the
values of `X` that the SAT solver proposes. A value near 99999 passes a few
stores. A value near 0 passes the whole chain: 300000 AIG ANDs and 13 to
16 s on the byte chain. Each further round, with a new value, adds about
100000 ANDs.

Bitwuzla's solve time without parsing, in seconds; x is the value in the
final model and rounds is the count of array lemmas. All 30 runs went at
once (load about 40):

| Formula | 0.9 x | rounds | solve | 0.11 x | rounds | solve |
|---|---|---|---|---|---|---|
| vs | 4 | 1 | 20.4 | 66706 | 1 | 33.7 |
| P | 4 | 1 | 19.3 | 99476 | 1 | 30.4 |
| Q | 99999 | 3 | 4.0 | 18 | 9 | 57.8 |
| R | 49859 | 1 | 14.9 | 99476 | 1 | 30.9 |
| A2 | 99999 | 3 | 4.1 | 18 | 9 | 52.8 |
| A | 99999 | 3 | 7.7 | 18 | 9 | 66.9 |
| B | 99999 | 3 | 6.3 | 18 | 9 | 59.8 |
| C | 99999 | 3 | 4.6 | 18 | 9 | 52.2 |
| G | 99999 | 3 | 4.4 | 18 | 9 | 56.4 |
| D | 99999 | 1 | 1.7 | 26 | 9 | 19.0 |
| E | 99999 | 1 | 4.0 | 18 | 9 | 19.8 |
| F | 99999 | 1 | 2.0 | 26 | 9 | 16.1 |
| es | 99999 | 1 | 4.4 | 26 | 9 | 20.7 |
| A2rev | 99999 | 3 | 52.2 | 18 | 8 | 4.5 |
| Drev | 99999 | 1 | 6.5 | 18 | 9 | 2.9 |

With the property at level 0, Bitwuzla 0.9 proposes `X = 99999`, the
largest value below 100000. The loop writes `a[99999]` last, so that is the
outermost store, and ESBMC's 2 s comes from this luck. With the stores in
reverse order, 0.9 is the slow one (A2rev 52 s, Drev 6.5 s against 1.7 s)
and 0.11 the fast one. With the property under a push or an assumption,
0.9 proposes 4 or 49859 and takes 15 to 20 s. That is the 4 to 5 times
between vs and Q that the first measurements found; the base-address
assertions (vs against P) and the models (5.0 s against Q's 4.7 s) do not
matter. A is slower than A2 most likely because the rewriter has 400000
`concat` indices to fold.

With the luck taken out, by confining `X` at level 0 to {0, 1} (low: the
whole chain) or to {99998, 99999} (high), with all 20 runs at once:

| Shape | 0.9 low | 0.11 low | 0.9 high | 0.11 high |
|---|---|---|---|---|
| A2: bytes, 96-bit index | 67.8 | 18.3 | 3.3 | 2.5 |
| B: bytes, 64-bit (H) | 33.8 | 18.9 | 4.6 | 5.8 |
| E: words, 96-bit | 9.0 | 6.7 | 3.0 | 3.9 |
| F: words, 32-bit | 6.0 | 5.1 | 2.0 | 2.8 |
| D: words, 17-bit (ESBMC) | 4.6 | 4.6 | 1.3 | 2.5 |

Here 0.11 is not slower than 0.9. On the byte chain it is 2 to 4 times
faster: it serves the four selects in one round, where 0.9 takes four.

## Bitwuzla 0.11 against 0.9

The two versions have the same defaults for every option both have
(abstraction on at 33 bits and wider, preprocessing on, rewrite level 2,
CaDiCaL). 0.10 dropped `pp-contr-ands` and `pp-elim-extracts` (both off by
default) and added quantifier options, `adc-sat-propagator` (off) and
`bv-cnf-scope` (adaptive). On the same formula, 0.11 is slower for two
reasons.

First, 0.11 proposes small values and then a new value in each round: 9
rounds on Q, D and es. No option changes this. With seeds 1, 2 and 3,
`adc-sat-propagator=1` or `rewrite-level=0`, D still ends at x = 26 after
9 rounds, in 20 to 22 s. The earlier runs found the same with abstraction
off (D 16.5 s against 16.2 s); turning preprocessing off made 0.9 slower
(Q 126 s). No Bitwuzla option reaches the SAT solver's choice. The move
from CaDiCaL 2.1.2 to 3.0.1 between the two is the likely source, but this
was not tested in isolation: 0.10 and later refuse a CaDiCaL older than
3.0.

Second, under `check-sat-assuming` (how Vereda solves), 0.11 does one round
and then spends 13 to 17 s in the last SAT call on the 900000 clauses. With
`bv-cnf-scope=permanent` that call takes 0.6 s (H-assume: the lemma is done
at 19.2 s and the answer comes at 37.2 s; with the option, at 19.8 s and
21.4 s). Under the default, `adaptive`, the lemma's AIG nodes take the
level of the assumption, so their clauses carry an activation literal;
that likely stops CaDiCaL from simplifying them (the issue's profile shows
its gate detection).

Vereda at a0a466f6, with a local hook that sets the option through
camada's `BitwuzlaSolver::configureOptions`, two runs each, load about 25:

| `bv-cnf-scope` | Solving time | Peak |
|---|---|---|
| adaptive (default) | 27.6 s, 28.5 s | 2.6 GB |
| assertion | 20.1 s, 19.3 s | 2.7 GB |
| permanent | 16.7 s, 17.0 s | 2.4 GB |

The last section says what the option does and measures it over the
suite; Vereda now sets it.

## Word elements

Words against bytes at the same index width: 18.3 s against 6.7 s for 0.11
on the whole chain (A2 and E, low); 57.8 s against 19.8 s and 19.0 s for
0.11 with nothing confined (Q against E and D). With `X` high, words gain
nothing, since parsing takes most of the time. So word elements cut the
solve about 3 times when the chain has to be walked, which leaves Vereda
at 5 to 7 s against ESBMC's lucky 2 s. Each round adds about as many AIG
ANDs either way; the difference is in the array check, which walks 400000
stores for four selects against 100000 for one.

What it would take, not built. The candidates are objects that already get
arrays of their own (#88) and that are only ever read and written whole:
every use in the getelementptr tree from the object is a load or store of
one integer type iN (N = 16, 32 or 64) at an offset that is a multiple of
N/8 (constant offsets, and symbolic indices scaled by N/8). No call or
intrinsic may take the object, since `PointerMayBeCaptured` does not count
memset, memcpy or memcmp, and its initialiser must be zero or whole iN
elements. That check is about 30 lines next to `ownsArrays` in
`walker.cpp`. In `memory.cpp`:

- `own()` records N and makes the arrays N bits wide, indexed by offset
  divided by N/8.
- The known-byte map keeps the object's bytes as now, and `flush()` writes
  the N/8 known bytes of an element as one store. `flush()` is where the
  test's 400000 stores come from today.
- A load or store of iN at the object becomes one select or store.
- `fillObject` builds the constant array of the byte repeated.
- The provenance array goes, since no pointer is stored there.
- An access through an unknown pointer (`readArrays` and `writeByte` with
  no constant object), which still considers the object, extracts a byte
  from the element, or reads, masks and writes the element back.
- `mergeSeparate` needs nothing.

That is roughly 150 to 250 lines plus tests. It is smaller than #72's
general word cells, because the rule leaves no narrow or unaligned access
except through an unknown pointer. The gain is confined to integer arrays
that long loops fill, which #36's loop summaries remove outright, and
`2026-11-23-word-sized-cells-measured-and-not-built.md` found nothing
outside that family in ReachSafety.

## Z3

camada's Z3 5.4.1 (through `Z3_eval_smtlib2_string`) gave no answer within
300 s on vs, Q, R, A2, D or es (es after renaming its `\guard_exec`
symbol, which Z3 rejects). It also gave none with
`rewriter.expand_select_store` on D, Q, H and es, with
`smt.array.extensional=false` on D, or, in 120 s, on D with `X` high.
ESBMC with `--z3` (Z3 4.13.3) and Vereda with `--solver z3` did not finish
in 300 s either (ESBMC's symbolic execution took 6.6 s and Vereda's walk
32.8 s). Z3's time grows with the length of the chain, not with `x`, so
this series uses shorter chains of the same program, at most 300 s each:

| Chain | Z3 |
|---|---|
| 100 words | 0.7 s |
| 300 words | 10.5 s (`X` low 10.5 s, high 9.3 s) |
| 1000 words | over 300 s, 7.2 GB |
| 100 words as 400 bytes at a 96-bit index | 92.4 s |
| 300 words on a nondet base array | 1.5 s |
| 1000 words on a nondet base array | 22.4 s |

On 300 words, `smt.relevancy=0`, `expand_select_store`,
`smt.array.extensional=false` and `smt.auto_config=false` all took 10 to
13 s; `sat.euf` ran past 60 s; `smt.array.weak` and the `sat` tactic
answered unknown. Z3 is no fallback for chains this long, in either tool.

## Conclusions

1. The solve is the read of `a[x]` through a chain of constant stores,
   which Bitwuzla resolves with lemmas whose size depends on the `x` the
   SAT solver proposes. ESBMC's 2 s comes from Bitwuzla 0.9 proposing
   99999 for a loop that writes upwards. ESBMC's own formula takes 20.7 s
   on 0.11, and on 0.9 a loop that writes downwards is the slow case.
2. Bitwuzla 0.11 is not slower once `x` is fixed. Here it proposes worse
   values, and under `check-sat-assuming` its adaptive `bv-cnf-scope` adds
   about 11 s in Vereda. `bv-cnf-scope=permanent`, one option set through
   camada's `configureOptions`, takes Vereda's solve from 28 s to 17 s.
   Over the suite it changes no verdict and cuts wall time by 4.5 %, so
   Vereda now sets it.
3. Word elements for objects read and written only whole cut the cost of
   walking the chain about 3 times, for roughly 200 lines, and still leave
   5 to 7 s.
4. Only removing the chain, with #36's loop summaries, gets this test to
   ESBMC's time without depending on which `x` the solver tries first.
5. Z3 cannot handle chains of this length.

## `bv-cnf-scope=permanent` over the suite

What the option does, from Bitwuzla 0.11's source
(`src/bitblast/aig/aig_cnf.cpp`, `src/solver/bv/bv_bitblast_solver.cpp`,
`src/api/cpp/bitwuzla.cpp`). `check_sat` with assumptions pushes a level,
asserts the assumptions there, solves, and pops before the next call that
changes the solver. Each AIG node gets clauses that define its CNF variable
(`x <-> a & b`), and the option picks the level they go at. With
`assertion`, the level of the first assertion that needs the node. With
`permanent`, level 0; only the clauses that assert a top-level node take
the assertion's level. With `adaptive`, the default, the highest level of
the node's children, except that a node an assertion of a pushed level
needs over inputs of lower levels takes the assertion's level, unless
enough such nodes were needed again after earlier pops. Clauses of a
popped level are disabled and their nodes encoded again when needed.

Vereda builds all its terms at level 0 and poses every check, in
`decide()` and in the walker's reachability checks, through
`checkSatAssuming`, so each check is a push and a pop inside Bitwuzla.
Under `adaptive` much of a check's cone goes at the pop and the next check
encodes it again. Under `permanent` it stays, and so do the array lemmas,
which go at level 0. That is sound: the clauses of a node only name a fresh
variable for a function of its inputs, so they exclude no assignment of the
inputs, and the lemmas hold in the theory of arrays. The cost is that
clauses of checks never asked again stay in the SAT solver. Vereda pops
only around the dump, before anything is solved, and `--incremental-bmc`
makes a new solver for each bound, so there is no other pop to consider.

Both binaries were built at cdbc223c, one with the option set in
`src/solver/solver.cpp`. Every test ran through `tests/run.py` (4096 MB
limit, 120 s timeout) with both binaries side by side, 24 tests at a time,
load 5 to 30 from other jobs; the 13 tests that timed out ran again with
285 s. Over the 3365 tests with a verdict in both:

| | adaptive (before) | permanent |
|---|---|---|
| Verdicts changed | | 0 of 6645 |
| Walk time (includes the walker's checks) | 429.7 s | 345.5 s |
| Solving time | 227.0 s | 238.2 s |
| Wall time | 1402.7 s | 1340.2 s |
| Sum of peak memory | 266.0 GB | 266.4 GB |

Both give 1905 SUCCESSFUL, 1346 FAILED, 114 UNKNOWN, 9 timeouts, 8 out of
memory and 3263 without a verdict (FUTURE tests that need flags Vereda
lacks). The tests whose wall or solving time moved by more than 10 % and
0.5 s, measured again side by side with `tests/bench.py --list` (seconds
and MB, before → after):

| Test | Walk | Solve | Wall | Peak |
|---|---|---|---|---|
| 13_ludcmp_new | 95.7 → 64.1 | 0 → 0 | 96.7 → 64.7 | 520 → 377 |
| 12_lms_new | 157.0 → 119.7 | 0 → 0 | 158.0 → 120.5 | 359 → 390 |
| array_loop_summary_decrement_fail | 11.6 → 11.9 | 29.1 → 17.8 | 52.5 → 39.4 | 2564 → 2411 |
| github_9785_int_base_stores_fail | 0 → 0 | 15.9 → 13.5 | 16.7 → 14.1 | 455 → 485 |
| nested_array_row_memcpy_at_cap | 0 → 0 | 8.5 → 8.1 | 9.4 → 9.0 | 446 → 444 |
| overflow_22_wordplay | 0.1 → 0.1 | 1.6 → 1.2 | 2.0 → 1.6 | 236 → 198 |
| github_302 | 64.7 → 55.4 | 17.8 → 36.5 | 83.2 → 92.5 | 633 → 726 |
| 10_bicycle_01 | 0 → 0 | 7.8 → 8.9 | 8.7 → 9.7 | 433 → 423 |
| github_9785_two_targets | 0 → 0 | 1.0 → 1.2 | 1.1 → 1.4 | 91 → 91 |
| github_9785_two_targets_fail | 0 → 0 | 1.4 → 2.2 | 1.6 → 2.3 | 94 → 94 |
| vereda/mem_inttoptr | 0 → 0 | 2.5 → 3.0 | 2.6 → 3.1 | 102 → 105 |
| esbmc-cstd/ispunct | 0 → 0 | 1.1 → 3.0 | 1.3 → 3.2 | 147 → 195 |
| esbmc-cstd/ispunct_bug | 0 → 0 | 0.8 → 2.8 | 1.0 → 3.0 | 135 → 179 |

`deep_binary_chain_pass` (49 → 60 s in the suite run) took 33.0 and 33.2 s
side by side, none of it in the solver. `array_loop_summary_decrement` and
`_counter` run out of the 4096 MB with both binaries; without the limit
neither finished in 295 s with either.

The gains come from many checks on one solver. 12_lms_new and
13_ludcmp_new have no `--unwind`, so the walker asks whether each loop
iteration is reachable, one check per iteration on a growing formula, and
`permanent` keeps the encoding from one to the next; github_302's walk gains
for the same reason. The losses are single checks: github_302,
ispunct_bug and the others above have one, and the same clauses at level 0
instead of under an activation literal send CaDiCaL down another path. With
a binary that took the mode from the environment, two runs each:

| | adaptive | assertion | permanent |
|---|---|---|---|
| github_302 walk | 66.1, 65.6 | 139.7, 141.8 | 54.2, 54.2 |
| github_302 solve | 20.1, 20.2 | 17.4, 17.4 | 37.5, 36.7 |
| github_302 peak MB | 637, 637 | 1955, 1952 | 684, 695 |
| ispunct_bug solve | 0.74, 0.78 | 3.26, 3.60 | 2.60, 2.63 |

`--incremental-bmc`, side by side, gave the same verdicts and bounds with
both. The 280 suite tests that use it take 3.2 s of solving in all, either
way. On heavier programs (solve, before → after): github_302, 3 bounds,
14.2 → 12.8 s and 355 → 266 MB; 13_ludcmp_new, 7 bounds, 0.15 → 0.14 s;
github_9785_int_base_stores_fail 12.5 → 10.8 s;
nested_array_row_memcpy_at_cap 7.1 → 6.8 s; 10_bicycle_01 7.7 → 8.7 s;
github_9785_two_targets_fail 1.4 → 2.2 s. overflow_22_wordplay did not get
past bound 2 in 290 s with either.

Adopted: no verdict changes, 4.5 % less wall time, the decrement test's
solve down from 29 to 18 s side by side, and the two slowest walks down by
a quarter to a third, against single checks that solve slower (github_302
by 19 s, a few small tests by 1 to 2 s). The ReachSafety tasks were not
run.
