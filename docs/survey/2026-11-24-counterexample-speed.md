# Counterexample and witness of a long trace

Recorded 2026-11-24.

`tests/esbmc/array_loop_summary_decrement_fail.c` with
`--no-summarize-array-loops` runs 100,000 iterations of `j -= 2; a[i] = j;`
and then fails an assertion. On main (Release, Bitwuzla), the run spent
about 5.6 s between printing "Solving time" and exiting, over a quarter of
its wall time. A timer at the solver's answer, after the counterexample,
after the witness and at exit split that into building and printing the
counterexample (3.7 s) and tearing down the solver and the term builder at
exit (1.9 s).

## Where the counterexample's time went

The trace has 600,009 steps, 500,005 of them assignments the model reaches.
For each, the printer asked the solver for the guard (`getBool`), then for
the value twice (`getBVInBin` for the bits, then `getBVUnsigned` or `getBV`,
which camada answers through `getBVInBin` again), and for a store into the
array also for the byte offset: about 1.6 million queries. Each one runs
Bitwuzla's `get_value`, which preprocesses the term again (rewriting,
variable substitution, lambda elimination; 40% of the time), and camada
turns the answer into a string: `getBool` through `bitwuzla_term_to_string`,
the SMT-LIB printer (14%), and the bit-vector getters through
`bitwuzla_term_value_get_str` and `strtoull` (10%). Yet the terms were few
or known: the 500,007 guards are two terms, one of them `true`; 500,004 of
the 500,005 values and all 100,000 offsets are constants the term builder
folded. Location strings (`SourceLoc::str`, 1%), the map of last values
(`std::map` keyed by strings, 5%) and the output were small.

`ir::joinedPath` and `ir::fileName`, which showed in the profile of the whole
run, are spent in the walk (each step's location; about 1% of the run),
not in the counterexample.

## The change

`solver::Model` (`src/solver/model.h`) reads values for both the
counterexample and the witness: a constant from `Terms::constant`, any other
term from the solver once, kept by term. One read gives a value's bits and
both its signed and unsigned reading, and the bits are spelled from the
`APInt` by hand (`APInt::toString` in base 2 was 16% of what was left). The
map of last values is a `StringMap`. The driver makes one `Model` per
violated check and hands it to both, so the witness asks for nothing the
counterexample already read.

`--witness` had a cost of its own, which only shows on a large file: for each
branching waypoint (one per iteration) `keyword` scanned the file from its
start to the waypoint's line, and for each `function_return` waypoint
`returnColumn` split the whole file into lines. Each input file now keeps
the offsets of its lines, read once.

The printed counterexample and the witness are unchanged. The outputs of
main and of this change, witness files included, were compared on every test
under `tests/vereda` and every imported test that expects `VERIFICATION
FAILED`, 1,928 runs, leaving out timing lines, the UUID and the creation
time: 1,922 were the same, 4 differed only in the name of a temporary
directory (`--dump-ir` output and `witness_outside_input`), and 2 ran past a
minute. The runs below gave the same output too.

## Measurements

Release build, the solve-to-exit split from a timer patched into the driver
for these runs only; each figure the median of three runs, in seconds, on a
machine shared with other builds (the solving time varied by a second or two
between runs). "cex" is from the solver's answer to the end of the
counterexample, "witness" the witness after it, "after" the solver's answer
to exit, "wall" the whole run. The decrement and counter runs are
`tests/esbmc/array_loop_summary_{decrement,counter}_fail.c` with
`--no-summarize-array-loops`; the `--witness` runs are the decrement loop
with a call to `reach_error` in place of the assertion, once in a file of 15
lines and once after 20,000 lines of other functions;
`struct_array_ite_new_index_fail` runs with its test's flags; the last is
20,000 iterations of `a[i] = __VERIFIER_nondet_int()`, whose values are all
symbolic.

| run | binary | cex | witness | after | wall |
|---|---|---:|---:|---:|---:|
| decrement_fail | main | 3.68 | - | 5.56 | 19.48 |
|  | this change | 0.85 | - | 2.56 | 16.22 |
| decrement, `--witness` | main | 3.66 | 0.42 | 6.19 | 21.77 |
|  | this change | 0.87 | 0.23 | 2.95 | 18.42 |
| same, 20,000-line file | main | 3.64 | 20.96 | 26.92 | 45.83 |
|  | this change | 1.00 | 0.26 | 3.80 | 21.79 |
| counter_fail | main | 3.80 | - | 5.15 | 14.96 |
|  | this change | 1.03 | - | 2.17 | 11.93 |
| struct_array_ite_new_index_fail | main | 0.25 | - | 2.22 | 31.14 |
|  | this change | 0.26 | - | 2.27 | 31.74 |
| 20,000 nondet stores, `--witness` | main | 0.64 | 0.19 | 1.23 | 3.69 |
|  | this change | 0.32 | 0.06 | 0.75 | 3.23 |

Of the 0.85 s left on the benchmark, 0.28 s is one query: the guard of the
assertion's step, the only guard that is not a constant, which Bitwuzla
preprocesses whole. struct_array_ite_new_index_fail prints four states;
its 2 s after solving are the teardown below.

The teardown at exit (about 2 s here) is unchanged: three quarters of it is
Bitwuzla freeing its nodes (`bitwuzla_delete`, the node manager's garbage
collection, `malloc_consolidate`), the rest `encode::Terms`' maps. Exiting
without the destructors once the verdict is printed (`std::_Exit` after a
flush) cut the benchmark's run from 16.7 s to 14.6 s in a prototype; it is
left for a change of its own, since it affects every run, not only those
with a counterexample.

## camada

After the change camada's string conversions are 1.5% of the time of a
counterexample of 20,000 symbolic values (before, about 25% of the
benchmark's, mostly `getBool` through the SMT-LIB printer); what remains is
Bitwuzla's `get_value` preprocessing each term it is asked for. A `getBool`
through `bitwuzla_term_value_get_bool` would halve each `getBool`, but
after this change few are left, so camada was not changed.
