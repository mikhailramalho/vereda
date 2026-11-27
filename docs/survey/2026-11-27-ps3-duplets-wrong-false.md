# Wrong FALSE on nla-digbench/ps3-ll and verifythis/duplets

Recorded 2026-11-27.

The BenchExec run of main at 29759530 (unreach-call, `--incremental-bmc`,
60 s, 6 GB) answered false on two tasks whose expected verdict is true:
`c/nla-digbench/ps3-ll` after 52.9 s and `c/verifythis/duplets` after 16.4 s.
Both reproduce on main at 9e58fa66 with the benchmark's command line
(`-m32 --incremental-bmc --property-file unreach-call.prp`, so k-step 2 and no
largest bound): ps3-ll answers FALSE at bound 8191, duplets at bound 127.

## The counterexamples are not executions

ps3-ll: `k = 9`, the loop runs nine times, the trace's values are y = 9 and
x = 285, and the violated check is the assertion after the loop,
`6x - 2y^3 - 3y^2 - y == 0`. With those values it holds (1710 = 1458 + 243 + 9).

duplets: `n = 536870911`, mkdup writes x at indices 61 and 45, finddup
returns at its first pair, i = 0 and j = 1 (both zero after calloc), and main
reads i = 0, j = 1. Every one of main's assertions holds for those values.
With `--unwind 127` and no incremental BMC the same false answer comes with
n = 3, mkdup at 1 and 2, finddup returning (1, 2).

Compiling each task with a `__VERIFIER_nondet_*` that returns the trace's
values and running it reaches no `reach_error` (ps3-ll with k = 9, duplets
with both traces).

## Bitwuzla answers sat on an unsatisfiable formula

The formula of duplets at `--unwind 100` (`--dump-smt-only`) is answered sat
by Bitwuzla 0.11.0 on its own, and `--check-model` on that answer fails with
"model check failed"; with `--rewrite-level 1` it is unsat. Turning off any
one preprocessing pass does not change it. For ps3-ll a standalone
k = 8191 formula is unsat, but a script that first checks the k = 4095
formula and then, in the same solver, the k = 8191 one (as incremental BMC
does) reproduces the sat answer and the failing model check. Inside Vereda,
`check-model` set on the solver fails at the same bound for both tasks, with
either CNF scope (`permanent` as Vereda sets it, or the default).

Bitwuzla's rewriter counts nested rewrites (a rule building a node that is
rewritten in turn) and, at 4096 (`Rewriter::RECURSION_LIMIT`), leaves the node
as it is; a debug build asserts there, as the limit is meant only to stop
rewrite cycles. A build of the same Bitwuzla printing when it reaches the
limit does so on both formulas, each time at an extract of a 32-bit or 64-bit
ite chain whose branches are constants: `BV_EXTRACT_ITE` turns
`extract(ite(c, k, e))` into `ite(c, extract(k), extract(e))` and recurses
into e, one level per link. With the limit at 2^20 both formulas are unsat
and the model checks pass. Not caching the node left at the limit does not
help, so the answer goes wrong from rewriting stopping part way, not from the
cache entry.

The chains come from the walker's joins. A block entered on n paths got each
phi and its memory as `ite(guard n, value n, ... ite(guard 2, value 2,
value 1))`, n links deep. duplets' finddup returns from inside its nested
loops, so its return block joins one arrival per pair i < j; at unwind 92
that is 4186 arrivals, and the stored i (a constant on each path) became a
chain of 4186 constants, stored to memory a byte at a time with an extract.
Unwind 91 (4095 pairs) is answered correctly and 92 is not. ps3-ll's loop
exit joins one arrival per iteration; y's exit value is a chain of 8191
constants, and Bitwuzla turns `2 * y` (a shift left by one) into a concat of
an extract of it. Bound 4095 was below the limit; 8191 is the next bound
k-step 2 checks. At 9efef765, whose run timed out on both tasks, the joins
were built the same way: the newer code only reached those bounds within
60 s.

## Fix

The walker now joins the arrivals at a block as a balanced tree: the value of
the right half if any of its guards holds, else of the left half. That picks
the last arrival whose guard holds, as the chain did, so the formula means
the same, but it is log n deep. The disjunction of the guards is a balanced
tree too. Function returns are joined in a chain still: a `ret` is outside
every loop, so a call has at most as many returns as its function has `ret`
instructions.

After the fix duplets answers UNKNOWN at bound 127 (the unwinding assertion
fails, the assertions hold) and ps3-ll passes bound 8191 and 16383 without a
violation. The balanced joins were also faster: ps3-ll's bound 4095 solved in
25.7 s instead of 65.3 s, and the duplets reduction at unwind 91 in 38.8 s
instead of 72.2 s.

The 42 unreach-call tasks of `c/nla-digbench` and `c/verifythis`, run as
the benchmark runs them (60 s, 2 cores each, here on a local machine), gave
the same answers before and after but for duplets, FALSE before and a
timeout after; ps3-ll timed out in both, as this machine reached bound 8191
only after 60 s. No correct answer was lost and none of the times changed
by more than 2.4 s.

`tests/vereda/join_many_arrivals.c` is duplets reduced (FAILED before the fix
in 3.4 s, SUCCESSFUL after in about 10 s); `join_many_arrivals_fail.c` keeps
a real bug. No reduction of ps3-ll small enough for the suite reached the
limit: its trigger needs the solver state of the bound before, and the
nonlinear check makes each bound slow. It joins at a loop exit, the same code
path as the test.

## What remains

Any term Vereda gives Bitwuzla that a rewrite rule descends more than 4096
levels can still meet the limit: a chain built by sequential code, such as
`if (c) v = k;` repeated inside an unrolled loop, is as deep as the number
of iterations. Raising or removing the limit is Bitwuzla's to do (camada
builds it from source); this change removes the depth that the joins added.
