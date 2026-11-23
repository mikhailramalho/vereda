# Milestone 0 survey and first measurements

Status: 2026-11-21. Step 1 of milestone 0 (plan, section 9): before building
the memory stage, find which IR features the candidate tasks use.

## Method

- Candidates come from ESBMC's SV-COMP 2026 ReachSafety results
  (`esbmc-kind`, run 2025-12-18). 6,633 runs ended in TIMEOUT or OUT OF
  MEMORY; 1,280 of them ran out of memory, most in DeviceDriversLinux64
  (525), Sequentialized (286), ECA (210) and Arrays (101).
- 30 tasks: the plan's named ones (the altera driver from ESBMC #9785, two
  fuzzle tasks, the two DeviceDriversLinux64Large tasks that timed out), plus
  out-of-memory tasks, one per sub-directory, at most 3 per category (6 for
  drivers). Concurrency is left out; the spike is single-threaded.
- Each task went through `vereda.py --dump-ir-only` (front end and pass list).
  A scanner listed the IR features beyond what the walker handles, and a run
  of the walker on each task recorded where it gives up.

**Caveat: ESBMC ran k-induction.** An out-of-memory run may have died in the
inductive step or the forward condition, which Vereda does not have. The
results only nominate candidates. Before a task enters the spike, rerun
ESBMC on it in plain BMC mode with the bound Vereda gets, and keep it only
if ESBMC still blows up there. The altera task's blow-up is in the k = 3
base case (#9785), so it qualifies.

**The "list model" tasks are not C.** The plan's list-model blow-ups are
ESBMC's Python list model (#10312); they drop out of a C spike.

## Bugs found and fixed

All three are in Vereda's front end or walker, not missing features:

| Problem | Tasks hit | Fix |
|---|---|---|
| A textual pipeline's InstCombine aborts when it needs more than one iteration | 8 of 30 | `instcombine<no-verify-fixpoint>`, as LLVM's own pipelines run it |
| Clang 15+ rejects old C (int-conversion, incompatible pointer and function pointer types) | 2 of 30 | relax the same warnings ESBMC relaxes |
| `__VERIFIER_nondet_int()` declared without a prototype hides the callee | 3 of 30 | dispatch a bodyless callee by name |

After the fixes all 30 tasks compile and load.

## What the tasks need

Number of tasks, out of 30, that use each feature:

| Stage | Feature | Tasks |
|---|---|---|
| memory | globals | 30 |
| memory | load and store | 26 |
| memory | getelementptr | 19 |
| memory | alloca | 17 |
| memory | memcpy, memset, memmove | 12 |
| memory | ptrtoint, inttoptr | 11 |
| memory | insertvalue on aggregates | 3 |
| calls | variadic functions | 10 |
| calls | indirect calls | 8 |
| constructs | inline asm | 9 |
| constructs | atomics | 2 |
| constructs | integers wider than 64 bits | 1 |
| floats | floating point | 4 |
| walker | irreducible control flow | 2 |

Other gaps:

- **Bit intrinsics:** bswap (5 tasks), ctpop (4), fshl (3), objectsize (3),
  prefetch (2), bitreverse (2), fmuladd (2, with floats). Each is a few
  lines in the encoder.
- **C library functions** (6 tasks, all drivers plus `wc`): snprintf,
  sprintf, bcmp, strcpy, strcmp, strlen and a long tail of file I/O. These
  need the operational models, which the spike leaves out.
- **Irreducible control flow** shows up in two Loops `-reducer` variants.
  That family is generated with gotos, so it is likely common there.

The walker run agrees with the scanner: every task without a verdict stops
at memory first (alloca, load, store or a pointer type), except one
hardware task that also needs fshl and the two irreducible ones.

## The named tasks

- **altera_jtaguart** (833 instructions): memory, plus two inline asm MMIO
  accessors (`movl` memory to register, and register to memory: a 32-bit
  load and store) and a call to `printk`, a variadic function defined in the
  task. No C library calls beyond malloc and free. Out of the spike, since
  inline asm is not supported (decided 2026-11-21).
- **fuzzle 30x30 and 40x40**: pure integer recursion, so Vereda runs them
  today. ESBMC solved both in 2026; they regressed only in the 2027 run.
- **sfc and amdgpu** (66k and 135k instructions): need everything above,
  including the C library. Out of reach for the spike.

## Proposed spike set

Tasks that need memory and nothing else:

| Task | Category | Instructions |
|---|---|---|
| array_assert_loop_dep | Arrays | 38 |
| bAnd4 | Arrays | 61 |
| sanfoundry_02_ground | Arrays | 88 |
| SpamAssassin-loop.i.v+cfa-reducer | Loops | 133 |
| uthash_JEN_nondet_test1-1 | uthash | 618 |
| pals_floodmax.3.1.ufo.BOUNDED-6 | Sequentialized | 751 |
| tdh_sys_config (cover_entry_havoc_object) | Intel TDX | 5,299 |
| Problem14 | ECA | 328,520 |
| Problem08_label00, Problem102_label00 | ECA | 936,032 |

Each still has to pass the plain-BMC check above. The ECA tasks are the
size test: close to a million instructions after the pass list.

## Scope this adds to the memory stage

- (object, offset) pointers, one byte array, globals with initialisers,
  alloca, malloc and free, load, store, GEP, memcpy, memset, memmove,
  ptrtoint and inttoptr.

Not supported: inline asm, so a task that uses it stays UNKNOWN. Left for
later: variadic functions, indirect calls, floats, irreducible control flow,
atomics, wide integers, the bit intrinsics, and the C library models.

## Plain-BMC check (2026-11-21)

ESBMC rerun with the competition's reach flags, `--incremental-bmc` instead
of k-induction, 15 GB and 900 s per task, each in its own systemd unit:

| Task | ESBMC, plain BMC | Kept |
|---|---|---|
| array_assert_loop_dep | FAILED at k = 1, 8 s, 510 MB | no |
| tdh_sys_config | UNKNOWN at k = 50, 10 s, 216 MB | no |
| sanfoundry_02_ground | out of memory at k = 1 | yes |
| Problem14 | out of memory at k = 1 | yes |
| SpamAssassin-loop | out of memory at k = 3 | yes |
| pals_floodmax | out of memory at k = 2 | no: needs function pointers |
| uthash_JEN_nondet_test1-1 | timeout at k = 1 | yes |
| Problem08_label00, Problem102_label00 | timeout at k = 6 | yes |
| bAnd4 | timeout at k = 22 | yes |

Two of ten blew up only under k-induction. The spike set is the seven kept
tasks.

## First measurements (2026-11-21)

Vereda at commit 191a09e, same limits. Vereda has no incremental mode yet:
a script runs `--unwind 1, 2, ...` as separate processes in one 900 s
budget, so lower bounds are re-walked at every step. Peak memory is the
process's, after the walk (walker plus the terms it built) and after
solving.

| Task | ESBMC, plain BMC | Vereda, bound reached | Walk peak | Solve peak |
|---|---|---|---|---|
| SpamAssassin-loop | out of memory at k = 3 | k = 20 | 83 MB | 474 MB |
| uthash_JEN_nondet_test1-1 | timeout at k = 1 | k = 2 | 2.0 GB | 2.2 GB |
| Problem14 | out of memory at k = 1 | k = 1 | 0.9 GB | 4.2 GB |
| Problem08_label00 | timeout at k = 6 | k = 2 | 4.1 GB | 5.4 GB |
| Problem102_label00 | timeout at k = 6 | k = 2 | 4.1 GB | 5.3 GB |

sanfoundry_02_ground and bAnd4 loop 100,000 times, so no bound short of a
full unroll says anything; their out-of-memory at k = 1 came from
`--goto-unwind`, which unrolls constant loops before symbolic execution.
At `--unwind 100001`, without `--goto-unwind`:

| Task | ESBMC | Vereda |
|---|---|---|
| sanfoundry_02_ground | symex 61 s, encoding 50 s, then out of memory at 15 GB | walk 99.97% done at the 900 s limit, 6.8 GB |
| bAnd4 | timeout, 482 MB | walk 99.98% done at the 900 s limit, 5.4 GB |

### What the numbers say

- Vereda never ran out of memory; its peak stayed under 7 GB. On the three
  tasks where ESBMC ran out of memory at a small bound, Vereda got further
  (SpamAssassin k = 20 against 3) or as far (Problem14 k = 1, uthash k = 2
  against a timeout at 1).
- ESBMC is ahead on the two ECA tasks (k = 6 against 2) and walks the full
  unroll of sanfoundry much faster.
- **The walk, not the solver, is the cost on the large tasks.** On the ECA
  tasks it takes 2.2 GB at k = 1 and 4 GB at k = 2 before the solver
  starts. These tasks keep their state in 23 `int` globals, read 244,000
  and written 127,000 times; in the one byte array each load is 4 selects
  and 3 concatenations, each store 8 array stores, and 262,000 branches
  merge both arrays at their joins. None of the globals has its address
  taken.
- **Walking slows down with depth.** About 0.7 ms per iteration at 8,000
  iterations of one loop, but some 400,000 iterations took over 15 minutes.
  A profile at 8,000 iterations puts nearly all the time in Bitwuzla's term
  construction (hash-consing, term export, rewriting) reached through
  camada; the walker's own code is under 1%. The likely cause, not yet
  confirmed, is selects over store chains that grow with every iteration,
  which the rewriter looks back through.

### Consequences for milestone 1

The core bet survives on memory but not yet on time. The fixes are the ones
the plan already lists, now with evidence for their order:

1. Folding as terms are built (plan, section 4.2), above all a load from a
   known store, so selects stop growing with the store chain.
2. Partitions (section 4.4), starting with globals whose address is never
   taken: each becomes a plain value, which removes almost all array work
   on the ECA tasks.
3. Incremental BMC (section 4.2), so a run stops re-walking lower bounds.

Not measured: Problem14 at k = 2. SeaHorn, SMACK and Symbiotic (plan,
milestone 0) were skipped. SeaHorn ships only as a Docker image, and the
Symbiotic (SV-COMP 2026) and SMACK (SV-COMP 2022) archives were not run
outside a container, so ESBMC is the only comparison here.

## Folding as terms are built (2026-11-22)

The first fix above, with the same tasks, bounds and caps (15 GB, 900 s).
Terms are now built through `encode/terms.h`, which shares equal terms and
folds constants, identities, an ite with a constant guard, extract over
concat, and operations on an ite whose branches are constants. A branch
whose guard folds to false is not walked. Bytes written at a constant
address stay in a per-state map and reach the byte array only when an
access at a pointer that may alias them needs it. A malloc result,
ite(ok, object, null), is split into one access per side, or one side when
the path guard already implies ok.

| Task | k | Walk before | Walk after | Solve before | Solve after | Peak before | Peak after |
|---|---|---|---|---|---|---|---|
| Problem08 | 1 | 14.9 s | 0.5 s | 5.2 s | 0.06 s | 2.4 GB | 0.7 GB |
| Problem08 | 2 | 27.9 s | 1.1 s | 190 s | 3.5 s | 5.4 GB | 0.7 GB |
| Problem08 | 3 | not reached | 5.3 s | | 28 s | | 1.8 GB |
| Problem102 | 2 | 28.1 s | 1.1 s | 76 s | 3.4 s | 5.3 GB | 0.7 GB |
| Problem102 | 3 | not reached | 5.3 s | | 28 s | | 1.7 GB |
| uthash | 1 | 49.5 s | 0.1 s | 15.2 s | 1.5 s | 1.2 GB | 0.2 GB |
| uthash | 2 | 260 s | 34 s | 115 s | 18 s | 2.2 GB | 1.0 GB |
| SpamAssassin | 12 | 0.03 s | 0.03 s | 20 s | 20 s | 0.2 GB | 0.2 GB |
| sanfoundry | 100001 | over 900 s | 61 s | | over 840 s | 6.8 GB | 3.4 GB |
| Problem14 | 1 | 5.0 s | 2.1 s | 569 s | out of memory | 4.2 GB | over 15 GB |

Every verdict is the unwinding assertion of loop 0 or 1, as before. On 170
small Arrays and reducercommutativity tasks and the 19 fetched spike tasks,
at `--unwind 5` without unwinding assertions, the verdicts match the build
before folding, except wc-2, which went from UNKNOWN to SUCCESSFUL: its
first loop runs 10 times, so at bound 5 nothing after it is reachable, and
folding now sees that instead of walking into a function it cannot handle.

Problem14 got worse. It runs 57,776 32-bit multiplications and remainders.
With byte arrays, Bitwuzla bit-blasted a stored value only when a read
needed it; with values, each global is an ite over every branch's
arithmetic and all of it is bit-blasted up front. Z3 also runs out of
memory on it. Bitwuzla's lazy abstraction of large arithmetic (option
`abstraction`) is the likely fix; camada does not pass solver options
through yet.

SpamAssassin is unchanged: its cost is arithmetic in the solver, not memory.

## Bitwuzla abstraction for Problem14 (2026-11-22)

Bitwuzla abstracts multiplication, division and remainder and refines them
lazily (option `abstraction`, on by default in 0.11), but only for terms
of 33 bits or more (`abstraction-bv-size`). Problem14's arithmetic is 32
bits. Setting the size needs camada's `configureOptions` hook, which is on
master but not in a release (0.23 has no hook and bundles Bitwuzla 0.9.1),
so Vereda was built against a local static camada 0.24 with Bitwuzla 0.11.0.

| Run | Problem14 k = 1, assertion check |
|---|---|
| Bitwuzla CLI on the dumped formula, defaults | out of memory (15 GB) at 181 s |
| CLI, abstraction off | out of memory at 150 s |
| CLI, `abstraction-bv-size=32`, property asserted | unsat, 111 s, 2.9 GB |
| CLI, `abstraction-bv-size=32`, property as an assumption | unsat, 62 s, 2.8 GB |
| Vereda, size 32 | timeout (900 s), 6.4 GB |
| Vereda, size 32, `pp-normalize` off | timeout (900 s) |
| Vereda, defaults | out of memory |

The CLI solves the same formula that Vereda times out on. Verbose output
shows the difference is in preprocessing: inside Vereda the rewrite pass
doubles the term graph and normalization then spends 36 s growing it to
2.3M nodes, while on the re-parsed dump both passes leave it unchanged. The
formula is the same; the term ids differ (the parser creates terms in
another order, and Vereda's term manager also holds the terms the walk
built and dropped), and Bitwuzla's rewriting orders operands by id. So
the CLI result depends on term order and is not something Vereda can rely
on.

The setting also costs elsewhere: uthash k = 2 solving goes from 40 s to
71 s, array12_pattern from 45 s to 54 s. Problem08, Problem102 and
SpamAssassin are unchanged, and the 189-task verdict check matches.

Not adopted. Problem14 likely needs the old behaviour back for this shape: a
global written under many branches was a store into the byte array, which
Bitwuzla bit-blasted only where a read needed it.

## Partitions and the merge rule (2026-11-22)

Partitions first. Counting flushes of known bytes on the spike tasks: an
access at a pointer with an unknown object flushes every known byte, but
that happens only in uthash (6 times, 2,500 bytes at k = 2); Problem08 and
SpamAssassin never flush. A select that also skips stores into another
known object, (obj, i) against (obj', j), which is what one array per
object would give, changed nothing on the spike tasks or the 189-task
check. Known bytes already keep most objects apart, so a points-to
analysis waits until a task needs it.

The merge rule instead: at a join, an object whose two sides hold different
values that multiply, divide or take a remainder goes to the arrays on both
sides, so Bitwuzla bit-blasts that arithmetic only where a read needs it.
Problem08 and Problem102 have none, so they keep their values.

| Task | k | Walk before | Walk after | Solve before | Solve after | Peak after |
|---|---|---|---|---|---|---|
| Problem14 | 1 | 2.1 s | 3.0 s | out of memory | 341 s | 4.2 GB |
| Problem14 | 2 | | 12.7 s | | out of memory | 15 GB |
| uthash | 2 | 36 s | 21 s | 18 s | 8.4 s | 0.6 GB |
| Problem08 | 3 | 5.4 s | 6.1 s | 30 s | 29 s | 1.8 GB |
| Problem102 | 3 | 5.2 s | 5.8 s | 29 s | 30 s | 1.7 GB |
| SpamAssassin | 12 | 0.03 s | 0.03 s | 15 s | 14 s | 0.2 GB |

Problem14 at k = 1 is back, faster than before folding (569 s). The check
is a walk over the value's terms that stops at array reads, memoized; a
first version capped its depth and called a capped term arithmetic, which
the memo then spread to every term above it and moved Problem08's
constant-valued globals too (walk 10 s, solve 73 s at k = 3). The 189-task
check gives the same verdicts and time.

## Incremental BMC (2026-11-22)

`--incremental-bmc` checks bounds 1, 2, ... up to `--max-k-step` (50) in one
process and stops at the first verdict that does not depend on the bound:
a violated assertion, or every unwinding assertion holding. The plan
(section 4.2) asks to keep the walk's state at bound k and continue from it.
This walker merges the paths that leave a loop, so a larger bound changes
the state after the loop and everything from there on is walked again;
ESBMC's incremental BMC also reruns symbolic execution per bound. Each bound
gets a new walk and solver; the IR is loaded and prepared once.

Wall time for bounds 1 to K, three runs side by side:

| Task | K | One process per bound | One process, new solver per bound | One solver, a scope per bound |
|---|---|---|---|---|
| Problem08 | 3 | 129 s | 54 s | 44 s |
| Problem102 | 3 | 119 s | 54 s | 45 s |
| uthash | 2 | 63 s | 32 s | 31 s |
| SpamAssassin | 12 | 43 s | 41 s | 61 s |
| bAnd4 | 20 | 0.40 s | 0.11 s | 0.05 s |

The saving over one process per bound is the IR preparation (the RERS
modules are 123 MB of IR). Keeping one solver and one term builder for all
bounds, with each bound's constraints in a push/pop scope, saved 17% on the
RERS tasks but cost 50% on SpamAssassin (61 s against 40 s, twice each), so
it was not kept.

On the verdict check's 188 distinct tasks (one is in both lists) at
`--max-k-step 10`, 120 s each: 4 FAILED and 4 SUCCESSFUL, all as expected;
139 UNKNOWN with no verdict up to bound 10, 9 unsupported, 32 timeouts. No
wrong verdict.

## Function pointers (2026-11-22)

Measured first: rerunning the 1,671 imported ESBMC tests Vereda failed,
the unsupported constructs by tests blocked were floating point (about
190), vector constants and operations (about 110, mostly GCC vector
extension tests), function pointers (48), variadic functions (25) and
atomics (20).

CBMC lowers function pointers in a GOTO pass that always runs
(remove_function_pointers): each call through a pointer becomes `if (fp ==
&f) f(args);` per address-taken function with a body whose type is
compatible (same arity; integers, enums, bools and pointers interchange;
other types by width; the return only if used), arguments and result
converted by byte extraction, and a final `assert(false)` ("no candidates
for dereferenced function pointer") plus `assume(false)`. A const pointer
resolved to one function becomes a direct call. ESBMC instead forks in
symbolic execution over the pointer's value set, filters with a copy of
CBMC's rule (keeping all targets when the filter would remove every one),
and by default (open world) skips a call with no target and leaves its
result nondet; `--closed-world-fnptr` ends that path.

Vereda follows CBMC's shape in `ir` (lowerCalls, before InstCombine) with
ESBMC's open world: the fall-through calls a bodyless declaration, so the
result is unconstrained and the path goes on. Every function whose address
is taken gets a one-byte object, so a function pointer is an ordinary
pointer. The walker folds the tests, so the precision matches ESBMC's value
sets without an analysis: for `p = c ? &f : &g`, `p == &h` folds to false
for every other h, and InstCombine folds `(c ? f : g) == f` back into c.
A direct call to a defined function of another type (across files, or a
definition without a prototype) is converted the same way; the walker used
to reject it. Such a call does not count as taking the function's address
(hasAddressTaken's IgnoreCastedDirectCall): with the two undeclared
__VERIFIER_ calls of array_loop_summary_decrement_fail counted, their two
extra objects took that test from 66 s to over 120 s.

Tried and not kept: dispatching in the walker (works, but the lowered form
reuses every direct-call path and shows in `--dump-ir-only`); exact LLVM
types only (misses a definition without a prototype called through a typed
pointer, and a call through a pointer without a prototype, which clang
emits with type `(...)`); ending the path when nothing matches (a wrong
SUCCESSFUL for `extern int (*cb)(int)`, whose target lives in code not
given).

Of the 48 tests, 46 now reach a verdict and 29 imported tests pass that did
not; the verdict check's 188 tasks give one new correct FAILED
(pals_floodmax BOUNDED-6, UNKNOWN before) and no other change. Still open:
a call to an unknown function changes no memory, as for every bodyless
function; ESBMC's own `__ESBMC_assume` and an undeclared `assert` are not
modelled; a `byval` argument is not copied, so a callee that writes its
by-value struct parameter changes the caller's.

## Floating point (2026-11-22)

A float is the bit-vector of its IEEE bits everywhere: in the value map,
in memory, in phis and selects, and for nondet values, so loads, stores,
unions and int<->float bitcasts need nothing new. An FP instruction views
its operands as camada's bit-vector encoded floating point
(FPEncoding::BV, through mkBVToIEEEFP and back with mkIEEEFPToBV), which
camada bit-blasts itself with hardware NaN payloads; the native FP sort
was not used, as its single abstract NaN makes the bits of a NaN written
to memory unspecified. Rounding is to nearest, ties to even; fesetround is
not modelled. fneg, fabs, copysign and llvm.is.fpclass are bit operations
in the term builder, so they fold. fptosi and fptoui truncate (camada's
mkFPToSBV; float_arith checks it). frem stays unsupported: it is C's fmod,
which truncates, and SMT-LIB's fp.rem rounds to nearest. x86_fp80 (long
double) is not an IEEE interchange format and stays unsupported.

Later the same day the default became the solver's own floating-point
theory (FPEncoding::Native), with `--fp2bv` for camada's encoding, as in
ESBMC. Values stay bit-vectors either way; only the view inside an
operation changes. Under the solver's theory a NaN result's bits are some
NaN, as camada's mkIEEEFPToBV documents, while `--fp2bv` keeps payloads
(float_nan_payload and float_nan_payload_native pin the difference). The
float tests give the same verdicts both ways and on Z3 and cvc5; Bitwuzla
is fastest, Z3 slowest (20 s against 0.5 s on float_intrinsics). With
`--fp2bv` translated by import_esbmc.py, 10 more imported tests pass.

Keeping floats as bits under the solver's theory turned out slow: SMT-LIB
has no float-to-bits operation, and camada emulates mkIEEEFPToBV on
Bitwuzla and cvc5 with an uninterpreted function tied by
`to_fp(fn(x)) == x`, so every FP result added a function application and
a constraint. 10_fir_new (`--unwind 34`) took 0.48 s to walk and did not
finish solving in 10 minutes. In native mode a float is now an FP term,
converted to bits only at memory, bitcasts, vector elements and
copysign's sign test; fneg and fabs are FP operations, and is.fpclass
uses the theory's class tests (one NaN, so either NaN class means any
NaN). x86_fp80 stays bits, with neg and fabs as bit operations.

| Build | Walk | Solve | Peak memory |
|---|---|---|---|
| 1266dcdf (fp2bv only) | 8.6 s | 12.8 s | 3.1 GB |
| native, floats as bits | 0.48 s | over 10 min | |
| native, floats as FP terms | 0.42 s | 0.20 s | 107 MB |
| `--fp2bv`, same build | 13.6 s | 20.8 s | 3.1 GB |

The float tests keep their verdicts and run faster native than with
`--fp2bv` (float_intrinsics 0.03 s against 0.08 s). One imported test,
native_fp_nan_sign, goes back to KNOWNBUG: it reads a NaN's sign bit,
which the theory does not fix.

Clang keeps sqrt and fma as library calls while errno is on, so a pass in
ir turns calls to bodyless libm functions with an LLVM intrinsic (sqrt,
fabs, floor, ceil, trunc, round, rint, nearbyint, roundeven, fmin, fmax,
fma, copysign, and the f and l variants) into that intrinsic; errno is not
set, as in ESBMC's models.

Imported ESBMC tests: 84 now pass (83 FUTURE, 1 KNOWNBUG), none regress.
Of the unsupported reasons, floating point is gone except x86_fp80 (12
tests); the largest left are vector global initialisers (110), atomicrmw
and cmpxchg (17), vector instructions and values, and variadic functions.
The verdict check's 188 tasks give one new correct SUCCESSFUL
(bary_diverge, UNKNOWN before) and no other change. Some promoted tests
(10_fir_new, about 38 s alone) come close to the 60 s limit for imported
tests and can time out under a full parallel run. Not yet done: printing
float values in the counterexample, constant folding of FP operations in
the term builder, fmod, and the rounding mode.

## Vectors (2026-11-22)

The 133 imported tests that stopped at a vector (110 at a vector global
initialiser, the rest at vector values and element instructions) are GCC
vector-extension tests, plus x86-64 calls that pass `float _Complex` and
small structs of floats as `<2 x float>`. Teaching the encoder every
vector instruction was not needed: LLVM's scalarizer
(`scalarizer<load-store;variable-insert-extract>`, after InstCombine)
splits vector arithmetic, comparisons, selects, phis, shuffles, loads,
stores and variable-index element accesses into scalar code. Left for
Vereda: vector constants in global initialisers, laid out element by
element one alloc size apart as the scalarizer's element pointers read
them, and vector values at call boundaries, which the scalarizer keeps.
Those are one bit-vector with element 0 in the low bits, the little-endian
memory layout; constant-index extractelement and insertelement are bit
extracts and splices. Vectors of i1 and of pointers stay unsupported, and
so does a bitcast between a vector and a scalar.

128 of the 133 tests now give the expected verdict and 133 imported tests
leave FUTURE, with none regressing; the verdict check's 188 tasks are
unchanged. Of the rest, two convertvector tests call an undeclared
assert, which Vereda takes for an unknown function, and two complex
division tests call compiler-rt's __divsc3, which has no model.

## Arrays per object for whole-object fills (2026-11-22)

`memset_whole_large_buffer` memsets two 16 MB heap buffers. Memory wrote
one byte per array store, so each memset became 16 million stores; the
walk passed 2.6 GB in 20 s without finishing. The 1 MB limit on byte
stores was meant to catch this but applied only to sizes the solver
works out, not to a constant size in the IR. ESBMC fixed the same test
(ee3bb61e54, #9991) with one constant array for a memset over a whole
array of scalars.

With one shared array for all objects that cannot be said: a range
inside it is no constant array, and SMT-LIB has no lambda. So, as a first
step towards the partitions of plan section 4.4, a fill that covers a
whole object of at least 1 KB (memset from offset 0 for the object's
constant size, calloc, a zeroed global) gives the object a data and a
provenance array of its own: constant arrays of the fill byte, indexed
by the whole pointer like the shared ones, so that an object separate on
one path only reads the shared arrays on the other at a join. A read at a
constant object uses its arrays; at another object, an ite over the
separate objects ends at the shared arrays; a write updates the separate
objects it may hit, and the shared arrays unless the object is constant
and separate. Known bytes flush to their object's arrays. The term
builder folds a read of a constant array to its value. Lazy range writes
in the shared array were considered and not used: every read through
stores, joins and flushes would have to see the range, and one the
walker does not build would miss it. The 1 MB limit moved into memory
and now also covers constant sizes, for fills and copies that go byte by
byte; initialisers keep no limit, as before.

| Test | Before | After |
|---|---|---|
| memset_whole_large_buffer, _fail | killed at 15 GB | 0.16 s, 73 MB, right verdict |
| multidim_padded_overflow, _fail | killed at 15 GB | 0.15 s, 70 MB, right verdict |

The array_loop_summary tests do not change: they unroll 100,000
iterations with a store each, another problem. The verdict check's 188
tasks and the spike tasks (uthash, Problem08, SpamAssassin) are
unchanged. A read past the end of a separate object now gives the fill
byte instead of an unconstrained one; that read is undefined and a
bounds check will report it, whatever its value.

## Atomics (2026-11-22)

Vereda runs one thread, so an atomic is exactly a plain load, an
operation and a plain store, and a fence orders nothing; ordering and
volatility do not matter. Every integer atomicrmw operation is the
LangRef formula on the old value, which is the result; pointer xchg keeps
provenance through memory's pointer stores. cmpxchg on integers stores
ite(ok, new, old) and returns {old, ok}, packed like any struct of
integers; a weak one may fail when equal, through a nondet bit. Floats,
i1 and cmpxchg on pointers stay unsupported. 12 imported tests pass; four
_fail tests now find the violation but expect ESBMC's wording of it. The
verdict check's 188 tasks are unchanged. Threads (plan section 8) will
need these to be real interleaving points.

## Library models (2026-11-23)

Rerunning the imported tests Vereda failed and collecting the bodyless
functions they call, by tests with a wrong verdict: pthread_create (120,
threads are out of scope), an undeclared assert (106), printf (40),
ESBMC's own intrinsics (__ESBMC_forall and others), sscanf, realloc
(18), strlen, strcmp, sprintf (14 each), the ctype tables, errno, time,
memcmp, bcmp, fmod and more.

An assert without <assert.h> is an implicitly declared function; ESBMC's
front end treats it as an assertion, and so does the walker now (with
the caller's line, which a C model would lose). The rest are C models in
models/, written from the C standard rather than ported from ESBMC: its
library models come from CBMC's, under the BSD 4-clause licence, and
Vereda is Apache 2.0. string.c has the <string.h> functions (memcpy,
memmove and memset stay with the walker), stdlib.c realloc (with
__vereda_object_size, the bytes to the object's end, from the walker),
libc.c glibc's ctype tables (the C locale, class bits byte-swapped as
_ISbit has them), the isX functions, tolower, toupper, __errno_location
and time, math.c isnan, isinf, isfinite and signbit as functions.

CMake builds them with the pinned clang (-fno-builtin, so nothing inside
is rewritten into other calls), runs Vereda's pass list over them with
opt, and links models-32.bc and models-64.bc. ir links the one matching
the module's pointer width after its own passes, with LinkOnlyNeeded, so
a program's own definitions win and a call InstCombine created (bcmp from
memcmp == 0) still finds its model. Two first tries did not hold:
linking in vereda.py before the passes left such calls bodyless, and
turning off the passes' knowledge of library functions to avoid them
also lost InstCombine's folding of cos, exp, pow, strtol and ffsl with
constant arguments, which 11 imported tests rely on.

Loops in the models run only as far as their result needs, as glibc's
do: strncpy pads with memset, strrchr hops between matches with strchr,
memcmp returns from inside the loop. The imported tests' --unwind bounds
are tuned to ESBMC's models, and every iteration costs the walk.

121 imported tests now pass. 16 that passed only because their
undeclared assert checked nothing now fail: they call functions without
a model (strstr, strspn, strcspn, strncat, strerror, ESBMC intrinsics) or
read uninitialised locals, which Vereda takes as a new nondet value at
each use. The verdict check's 188 tasks are unchanged. A read of the
ctype table at a nondet index costs the solver about 2 s.

## Uninitialised locals (2026-11-23)

mem2reg turns each read of an uninitialised local into undef, which the
encoder makes a fresh value per use, so `int y; a[0] = y;
assert(a[0] == y + 1)` failed; ESBMC and plan section 4.5 take such a
local as one nondet value. Before mem2reg, each static local's scalar
leaves (integers, pointers, half/float/double/fp128, struct fields; not
arrays, which stay in memory and read the same nondet bytes, nor long
double or bfloat, which locals only move as bits) are stored the result
of a bodyless `__VERIFIER_nondet_uninitialised.<type>` call, one fresh
value per call. freeze poison was the first idea, but InstCombine may
replace it by any constant. The debug record mem2reg makes for that store
shows the start value at the declaration, as ESBMC does when the local is
read while uninitialised; when it is written first, the record is removed
(corrected the same day: at first every such record was).

With the locals no longer undef, two intrinsics InstCombine used to fold
stay: llvm.objectsize and llvm.is.constant, now lowered as the back end
does (unknown: -1 or 0, and false). 40 imported tests pass; 8 that passed
by luck now fail for reasons Vereda does not model yet (pthread_exit
ending the path, __ESBMC_forall, __ESBMC_r_ok, ESBMC's alignment check).
The verdict check's 188 tasks are unchanged.



## Performance check on pull requests (2026-11-23)

68dfce6f took 10_fir_new from a 21 s run to a solve that does not end, and
no test failed: the imported suites skip a run past their time limit, so
tests.yml only sees verdicts. `.github/workflows/perf.yml` now builds a
pull request's base and head on one runner and `tests/bench.py` runs
the tests in `tests/bench.txt` with both binaries side by side, so the
runner's noise hits both alike, and compares them relatively: head fails
when it is 1.5 times slower and 2 s more, uses 1.5 times the memory and
200 MB more, times out or is killed for memory where base was not, gives
another verdict or none. A time or memory regression runs once more and
counts only if it shows again.

The 23 tests were picked from the 160 with the longest ctest times by
running each at 3b0eab7c, one per stage: floating point (10_fir_new,
10_bicycle_01, Float27, math_log02), memory (array_loop_summary_counter_fail,
the only one of its family under a minute, at 40 s and 1.4 GB;
memset_whole_large_buffer), the walker (github_306), Z3 (github_2312_5),
vectors and function pointers. Many candidates time out at 60 s or stop at
an unsupported construct; deep_binary_chain_pass takes 20 s, but in clang.
At 3b0eab7c a run of the list against itself took about 4.5 minutes on an
80-core machine, two of them in 10_fir_new, whose solve grew until the OOM
killer stopped it; since f4e1abde kept floats as FP terms it takes 0.7 s,
and at ee26cffa the list takes 1 m 41 s against itself and flags nothing.
Several tests became much faster with those commits (math_log02,
github_1600, Float27 under 0.2 s); they stay, as cheap guards against a
large slowdown.

Comparing 1266dcdf with 68dfce6f flags 10_fir_new (killed for memory after
137 s, 40 s before), math_log02 (x9), native_fp_arith_fail (x12) and
github_306 (walk 0.06 s to 6.4 s), all from the solver's FP theory, and
array_of_vector_symbolic, UNKNOWN before vectors and SUCCESSFUL after.
Float27 slows from 0.7 s to 2.4 s, under the 2 s margin. With ee26cffa as
base and 3b0eab7c as head, it flags 10_fir_new (head timed out),
math_log02 (x16) and github_1600 (x20, memory x4.8), the work of
f4e1abde and ee26cffa seen backwards.

## Stores in counterexamples (2026-11-23)

The trace showed only values in registers (debug value records), so a
store into memory was missing: for `a[0] = f(a[0])` with a global `int
a[1]`, the only line was `f`'s parameter `a` receiving its argument,
which read as the array. ESBMC shows `a[0] = 1` and no parameter binding.
Now a source store into a named variable kept in memory (a global, or a
local with a declare record) is a trace step carrying the variable's debug
type and the store's offset term; the witness evaluates the offset in the
model and spells the element or member written (`m[1][2]`, `s.y`, `g[i]`
with the index the model chose). A union member is the first one of the
store's size; a bitfield store, which writes the whole unit, a partial
write and an array of variable length are not shown, rather than named
wrongly. A parameter's record of its own argument is dropped; another
value, another parameter included, stays. Stores through a pointer to a
heap object, which ESBMC spells `p->x`, are not shown yet.

## Violation witnesses (2026-11-23)

`--witness FILE` writes the first counterexample that reaches
`reach_error` as a violation witness in the YAML format, version 2.0 (sv-witnesses,
`format/schemas/archive/v2.0/violation-witness-2.0-schema.yml`). The
witness is one `violation_sequence` entry: a `function_return` waypoint
for each integer a `__VERIFIER_nondet_` function of known sign returned
on the path, with the constraint `\result == v`, then the `target`. This is the shape Symbiotic
writes, and the cheapest that still fixes the path: the walker already
binds a fresh term at each such call, so the trace gains a `Call` step
with that term, and the witness reads its value from the model. Branching
and assumption waypoints would need the walker to keep branch locations
and source expressions; they are left until a validator needs them.

Decisions, from the 2.0 schema and the guide (version 2.4.1-dev):

- `format_version` is "2.0", the version every validator reads. Later
  versions only add to it.
- A `function_return` waypoint points at the `)` that closes the call.
  The debug location gives the start of the callee name, so the writer
  checks that the name is there and scans the line for the matching
  parenthesis. When it is not there (a call a macro expands to is located
  at the macro's name) or the call spans lines, the column is left out,
  which the format reads as the first suitable place on the line.
- The `target` points at the statement that violates the property. For
  unreach-call that is the call to `reach_error`, not the `__assert_fail`
  inside SV-COMP's `reach_error`, so the walker keeps the call being walked
  and the trace's check step takes its location. CPAchecker and Symbiotic
  put the target of the guide's example at that same call.
- The value goes in the callee's sign, by the SV-COMP name: a bodyless
  function has no debug type. The sign matters for types narrower than
  int, which C promotes before the comparison. Only names whose sign C
  fixes get a waypoint (`int`, `long`, `uchar`, `size_t` and the like);
  `pthread_t`, `sector_t` and other typedef names, and other bodyless
  functions, get none. Pointers, floats, values wider than 64
  bits and INT64_MIN, which has no decimal constant, get no waypoint;
  a witness with fewer waypoints is still valid, only less precise.
- Waypoints point only into the input files: the compile units' main
  files, hashed with SHA-256. Calls in headers, in the library models
  (built without -g) and the nondet calls for uninitialised locals (no
  debug location) are left out. When the target is outside them there is
  no witness, and vereda says why: a validator rejects a file the task
  does not list.
- The specification is always unreach-call from `--entry`, so a witness
  is written only when the violation is a call to `reach_error`, bodyless
  or with the check inside it. A failed `assert`, `__VERIFIER_assert`,
  sanitizer check or unwinding assertion gets none. Under
  `--multi-property` the first counterexample that reaches `reach_error`
  is the one written.
- Follow-up: SV-COMP's `.i` tasks carry linemarkers, so debug locations
  name the original `.c` file while the task's input file is the `.i`.
  Today such a witness is refused, as the target is outside the input
  files. Mapping locations back to `.i` lines is the fix, and it matters
  for most SV-COMP tasks.
- Correctness witnesses are not written. They need loop invariants, and a
  bounded proof has none to give.

The witness for the guide's `unsafe-program-example.c`, with an SV-COMP
`reach_error`, passes the 2.0 JSON schema, and its columns and target match
Symbiotic's. The linter (`witnesslint`) was not run: it needs packages (libclang,
pycparser) that are not installed here.

Counterexample replay moved to a later milestone. Notes for that work:
the prepared module, not the clang output, should run under lli, so the
run follows the semantics that were checked; the nondet values can come
from the same `Call` steps, served in walk order (on one path, walk order
is execution order). The call sites, not the declarations, are the place
to substitute values, since K&R declarations are variadic. A replay
should mark the violated check's instruction (which `Check` would have to
keep) and count it reached only when its condition fails there (the
argument of `__VERIFIER_assert`, true for `reach_error`). It cannot be
faithful where the walker's model differs from libc: malloc failing,
unknown externals with side effects, `__vereda_object_size`, and pointer
values.

## SV-COMP property files (2026-11-23)

`vereda.py --property-file f.prp` reads the property, as SV-COMP and
benchexec pass it, and the binary's `--property` counts only that
property's failures. A failure that does not count ends its path, as the
abort it is, and the run prints ESBMC's wrapper verdict (`TRUE`,
`FALSE_REACH`, `FALSE_OVERFLOW`, `UNKNOWN`) after its own.

- unreach-call (`G ! call(reach_error())`): a call to reach_error is the
  violation and its body is not walked, as ESBMC's
  `--enable-unreachability-intrinsic` does. assert, `__assert_fail`,
  `__VERIFIER_error` and `__builtin_unreachable` are not violations; a
  failed assert assumes its
  condition. Only reach_error is accepted, the name current tasks use.
- no-overflow (`G ! overflow`): clang compiles with
  `-fsanitize=signed-integer-overflow` and `-fsanitize-trap`, and only its
  checks count. SV-COMP's overflow is a signed result out of range; clang's
  shift checks would also flag a negative left operand, so they are left
  out. An .ll or .bc input is refused: its checks would have to be
  compiled in already.
- Anything else, valid-memsafety among them, is refused with an error:
  Vereda has no memory-safety checks yet (plan section 7.2), and a
  run that ignored them would answer TRUE without looking.

A failed unwinding assertion gives `UNKNOWN`, never a FALSE. The parser
takes one CHECK line; the memory-safety file has three and is refused
anyway. run.py replaces `%S` in RUN lines with the test's directory, so
the tests can name the property files in `tests/vereda/properties/`.
