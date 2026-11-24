# An LLVM-IR verifier: plan

Status: draft, 2026-11-20; milestone 0 and review notes added 2026-11-21. Nothing here is built or measured yet unless it
says so.

## 1. Purpose

Build a bounded model checker and k-induction prover for C (later C++) that
works on LLVM IR. It is a new tool next to ESBMC, not a replacement:

- ESBMC keeps Python, Solidity, Java/Kotlin and CUDA.
- ESBMC serves as a second opinion: two engines built differently catch each
  other's wrong verdicts.
- Whether ESBMC's C path later moves onto the new engine is decided only after
  the new engine has proved itself.

Long-term scope: reachability, memory safety (including memory tracking),
overflow, termination, k-induction, correctness and violation witnesses,
witness validation and precise concurrency for SV-COMP; once that work is
done, Test-Comp test generation and then C++.

## 2. Why a new tool

ESBMC's limits come from its architecture, not from bugs that will run out.

- **Typed memory.** ESBMC keeps objects with their C types, so each
  dereference rebuilds a typed value from a byte offset across struct, union,
  array and bit-field layouts. In about a month, roughly 15 fixes landed in
  `src/pointer-analysis/dereference.cpp` for this one class of bug (#10061,
  #10079, #10091, #10102, #10134, #10137, #10150, #10167, #10893, ...). LLVM
  IR with opaque pointers is untyped bytes; a byte-level memory has no such
  bugs.
- **Struct state retrofitted.** Field-sensitive SSA (#7521,
  `--field-sensitive-ssa`) is phase 1 of 4 and still collecting follow-up
  fixes (#10839, #10864). LLVM's `mem2reg`/SROA does the equivalent before we
  start.
- **Scale.** Symex had several quadratic costs (#9169, the guard work, the
  per-member store encoding left over from #10541/#9785).
- **Analyses.** GOTO is not SSA, so every analysis (Andersen, intervals, loop
  summaries) rebuilds what LLVM provides: dominators, LoopInfo, SCEV,
  MemorySSA, capture tracking.
- **Pointers from integers.** ESBMC's (object, offset) pointers need extra
  machinery for each integer-to-pointer cast (#10541).
- **Precise concurrency.** The encodings in §8 need address partitions and
  known shared cells up front, which a new design can have from the start.

What the new tool costs, and does not fix:

- Breadth: only C and C++. Other languages stay in ESBMC. Rust is not free:
  its IR brings its own panic paths and intrinsics.
- Correctness witnesses: invariants must be written over C source variables;
  after `mem2reg` those are SSA values, so mapping back through debug info is
  real work.
- Fifteen years of semantic corner cases: many vanish with byte memory; the
  library models, libc behaviour and SV-COMP conventions do not.
- The second-system risk: LLBMC (last release 2013, no public source) is a
  sound design that died when its authors moved on. The tool has to win
  somewhere measurable early.

If the main pain is frontend upkeep (Python, the irep2 migration), this tool
does not help, and the effort belongs inside ESBMC.

## 3. What we reuse

- **camada** for the solver backends.
- **ESBMC's C operational models.** They are C, so clang compiles them; the
  `__ESBMC_*` and `__VERIFIER_*` intrinsics need mapping.
- **ESBMC's regression suite.** About 3,400 C tests with expected verdicts.
  Many `test.desc` files match ESBMC-specific flags and messages, so sort
  early which ones translate (verdict-only tests do) before counting on it as
  a day-one suite.
- **Clang's sanitizers** (`-fsanitize=signed-integer-overflow,shift,bounds,...
  -fsanitize-trap`) insert undefined-behaviour checks at source level, before
  any optimisation.
- **LLVM's analyses**, but only those that do not rely on undefined
  behaviour: `mem2reg`/SROA, SCEV (with the caveat in §7.4), LoopInfo,
  dominators, capture tracking.
- **ESBMC's `andersen.cpp`** as a reference for the soundness rules of
  points-to (#9484): stores through "points anywhere" reach every addressed
  object, floats and vectors carry addresses like integers, an unmodelled
  value keeps its operands' targets.

Not reusable as they stand:

- LLVM's alias analysis. BasicAA answers pairwise queries within one
  function; TBAA assumes strict aliasing, which many SV-COMP programs break;
  the CFL Andersen/Steensgaard passes were removed from LLVM.
- ESBMC's Andersen. It is field-, offset- and context-insensitive and cannot
  split memory into partitions.

## 4. Architecture

```
C source
  │  clang -g -fno-finite-loops -fsanitize=...,-fsanitize-trap [-m32|-m64]
  ▼
LLVM IR (loops intact)
  │  short pass list: mem2reg, SROA, careful InstCombine, loop-simplify, LCSSA
  │  then strip nsw/nuw/exact/inbounds/noundef
  ▼
static analyses on the same IR
  │  points-to partitions, shared cells, per-property slicing
  ▼
walker: unroll + per-step points-to + encode, in one pass
  │
  ▼
camada → SMT solver → verdict, trace → witness, replay with lli
```

### 4.1 Front end and pass list

- `-fno-finite-loops`: clang otherwise marks C11 loops with a non-constant
  condition (C11 6.8.5p6) with `llvm.loop.mustprogress`, and every C++
  function with `mustprogress`. LoopDeletion then deletes such loops when they
  have no side effects, and SCEV assumes them finite. Any `mustprogress` left
  in the IR is an error.
- Insert the checks first (clang sanitizers, `llvm.*.with.overflow`, explicit
  error branches), then strip the no-wrap and in-bounds flags. Otherwise
  InstCombine folds the checks away (`x + 1 > x` becomes `true`).
- Keep the pass list short so that every conditional branch still maps to a
  source condition. No SimplifyCFG merging of branches. The operands of `&&`
  and `||` need their own marks (ESBMC needed the same fix in its GOTO
  conversion).
- Keep debug info from clang to the trace: locations, `dbg.value` and
  `dbg.declare` records, and the debug types (LLVM integers carry no sign).

### 4.2 The walker

The tool does not unroll the IR; memory is the main concern. One walker,
in the style of CBMC/ESBMC symex, runs over the rolled IR:

```
walk(region, ctx):                      # ctx = call stack + iteration numbers
  for block in reverse post-order of region:
    guard[block] = OR of (guard[pred] AND edge condition)  # merge at joins
    for inst in block:
      val[inst] = encode(inst, val, ctx)   # an SMT term, folded on the spot
      pts[inst] = points-to step           # same pass
    if block heads an inner loop L:
      for i in 0..k-1:
        header phis = (i == 0) ? entry values : previous latch values
        walk(body of L, ctx + i)
        collect exit values under the guard of leaving at step i
      unwinding assertion: the guard of reaching step k is false
```

- Calls: the callee's body is walked in a new context; the IR is not inlined.
  Recursion is bounded by depth.
- Folding as terms are built: constants, `ite` with a constant guard,
  `same_object` decided from the points-to set, a load from a known store. A
  branch whose guard folds to `false` is not walked. This replaces the
  clean-up LLVM would have done after unrolling.
- Memory used: the current iteration's map from values to terms and
  points-to sets (with LCSSA, only header phis and loop exits cross
  iterations, so earlier maps can be dropped), plus the solver's hash-consed
  formula. No IR copies, no dominator trees or MemorySSA over unrolled code.
- k-induction: the same walker, with the loop's header phis and the memory
  partitions it writes set to fresh values.
- Incremental BMC: keep the state after step k and continue to k+1, with one
  solver assumption literal per unwinding assertion. Nothing is walked twice.
- Irreducible control flow: rejected at first.

### 4.3 Points-to, two layers

- **Static, whole program, before the walk:** a sound, field-sensitive
  points-to analysis (our own, or SVF after a soundness review) chooses the
  memory partitions and the shared cells. Anything it misses is a possible
  wrong TRUE, so it must over-approximate: `inttoptr`, byte-wise pointer
  copies and unmodelled externals point anywhere.
- **Per step, inside the walk:** each pointer is an SSA value, so its set is
  computed once in topological order (GEP and casts carry the set with the
  offset shifted; phi and select take the union). Pointers kept in memory use
  one memory version per store, and loads read the version they see. This is
  as precise as ESBMC's value sets and cheaper, since nothing is copied. A
  load through `p` with `pts(p) = {a, b}` becomes
  `ite(obj(p) == a, A[off], B[off])`.

### 4.4 Memory model

- Pointers are (object, offset) from day one, with an object table holding
  each object's alive bit, size and base address. ReachSafety does not need
  the table; memory safety does, and changing the pointer form later is a
  rewrite.
- `ptrtoint` is the object's base address plus the offset. `inttoptr` takes
  "may point anywhere" from the static points-to.
- Base addresses are ordered and do not overlap, so pointer comparison across
  objects has a meaning.
- Memory is byte arrays, one per partition. Wide loads concatenate bytes;
  partitions whose accesses keep to one type win the speed back. This is the
  region model SeaHorn (sea-dsa) and SMACK (DSA) use.

### 4.5 Design choices to fix in milestone 1

1. (object, offset) pointers with the object table (§4.4).
2. Debug info kept end to end, including debug types for signedness.
3. A short pass list, so branches map to source conditions.
4. "One loop iteration as a transition relation" as a first-class call: the
   state is the header phis plus the written partitions, before and after one
   pass through the body. k-induction, the recurrent-state check, ranking
   functions, invariant checks and loop summaries all use it.
5. Every check is tagged with its property and source location, so one walk
   checks several properties (one assumption literal each), and a verdict
   counts only a failed check of the property asked for. (ESBMC's
   recurrent-state check had a bug where any failed check counted.)
6. Every nondet value is tagged with the source call that made it, for
   witnesses and test generation.
7. An ABI rule: clang packs small by-value structs into `i64` arguments, which
   looks like a pointer-to-integer cast to points-to, so pointers inside such
   structs would point anywhere. Undo the coercion, or make the analysis see
   through it. Build `-m32` and `-m64` for SV-COMP's ILP32 and LP64 tasks.
8. Pin one LLVM version, upgrade on purpose about once a year, and keep the
   code that touches LLVM's API thin. LLVM's C++ API changes every six months;
   LLBMC stalled on an old LLVM, and ESBMC carries support for LLVM 11–21.

## 5. Static pre-analyses

Each must over-approximate, or anything it drops is a possible wrong TRUE.
SAF (Static Analyzer Factory) is a bug finder and may be unsound, so it is a
source of ideas, not of code. Order:

1. Field-sensitive Andersen: memory partitions (separate SMT arrays), indirect
   call targets, lock identity (must-alias only, for §8.4).
2. Escape/shared analysis on top of it: picks the partial-order events and
   race candidates for §8.
3. Value flow on memory SSA: for each read, its list of candidate writes, so
   ranks are only needed within each list; a cone of influence per property.
4. Slicing on the IR before the walk.

Later: IFDS/IDE summaries (escape, nullness, uninitialised; lock typestate for
API misuse; must-lockset joins by intersection, so not plain IFDS);
nondet dependence, to run concrete code before encoding and to pick witness
values; intervals and octagons to discharge easy assertions before BMC.

ESBMC needs the same field-sensitive points-to for phase 2 of its
field-sensitive SSA, so one sound analysis could serve both tools.

## 6. Invariants

- Crab, with PPLite for polyhedra. APRON is in maintenance only and ELINA has
  been inactive since 2023.
- isl and SCEV for affine loops and array dataflow (not Polly). This follows
  ESBMC's array loop summaries (#10563): summarising `a[i]`-only loops solved
  Arrays tasks that unwinding cannot; reductions (sum, max/min, flags) are the
  next step there.

## 7. Properties

### 7.1 Reachability

Milestone 1, with violation witnesses.

### 7.2 Memory safety and overflow

- Valid dereference: at each load and store, the object is alive and the
  access is within its size.
- Valid free: offset 0, heap object, still alive.
- memcleanup: no heap object alive at the end.
- memtrack: reachability from globals (and, by SV-COMP convention, main's
  locals) at exit. Only partitions that may hold heap pointers need scanning.
- Overflow: clang's sanitizer checks.
- Clang's ASan works by shadow memory at run time and does not help an SMT
  encoding; these checks are ours.

### 7.3 k-induction and correctness witnesses

The walker with havoced loop state (§4.2) and invariants from §6. Correctness
witnesses write loop invariants over source variables, mapped from header
phis through `dbg.value`, and from memory through `dbg.declare`.

### 7.4 Termination

LLVM has no termination prover. Order:

1. SCEV trip counts (`getBackedgeTakenCount`,
   `getSymbolicMaxBackedgeTakenCount`) prove easy loops. SCEV relies on the
   no-wrap flags, so a count is sound only when overflow is ruled out
   separately (SV-COMP termination tasks are meant to be free of undefined
   behaviour, or we prove it).
2. The recurrent-state check: save the loop state at a nondet visit to the
   loop head and assert that no later visit repeats it; a failed assertion is
   a reachable cycle. This is the method of Metta et al.,
   "A shallow dive into the depths of non-termination checking for C
   programs", arXiv:2409.12985, which the SV-COMP tool PROTON runs. In SSA the
   loop state is just the header phis plus the written partitions, so the
   write-collection bugs of ESBMC's version (PR #10865) cannot occur.
3. Ranking synthesis over the transition relation (§4.5 choice 4). SCEV's
   `{start, +, step}` recurrences are ready candidates; harder loops get an LP
   synthesis. An inner loop's invariant must hold every time the inner loop is
   entered, not only in the first outer iteration (ESBMC #10857).

`willreturn` from FunctionAttrs is a weak filter. AProVE and llvm2KITTeL use
LLVM only as a front end and do the termination reasoning themselves.

### 7.5 Witnesses and validation

- Violation witnesses: from the walker's trace, debug locations, branch marks
  and tagged nondet values.
- Validating violation witnesses: the same walker with the waypoints as
  constraints, matching source line and column to IR blocks.
- Validating correctness witnesses: splice each invariant into the source at
  its location as a check and let clang compile it, rather than writing a C
  expression parser.

## 8. Concurrency

The goal is a precise encoding: all interleavings of the unwound program, not
a bounded number of context-switch rounds. None of these designs is built or
measured.

### 8.1 Candidate encodings

1. CBMC-style partial order, with from-read edges through a per-read source
   clock: precise, O(R·W) instead of cubic.
2. Rank plus SMT arrays: per address, writes get distinct ranks (injectivity
   through an inverse array), a clock array strictly increasing over the
   ranks, and a read is `V[s(r)]` with `T[s] < c(r) < T[s+1]`. The formula is
   linear; the solver does the pairwise work lazily (Bitwuzla lemmas). An
   atomic read-modify-write is a read with `s(r) = ρ(w) − 1`. Needs each
   write's address known in advance. No published source found for this. The
   closest is Horn and Kroening, "On partial order semantics for SAT/SMT-based
   symbolic encodings of weak memory concurrency" (arXiv:1504.00037), which
   cuts the partial-order constraints from cubic to quadratic.
3. Sort by (address, clock) with a Batcher network, then a linear scan:
   precise, propositional, O(n log² n), handles symbolic addresses.

### 8.2 Lock-protected data

A must-lockset analysis on the IR (ignoring accesses before the first thread
creation and after the joins) finds cells always guarded by lock L. Each
critical section of L becomes one step with one clock; sections get distinct
ranks, and the protected state flows `In(B) = Out[ρ(B) − 1]`. Lock variables
vanish and the cost is O(number of sections), precise by the mover argument.
Caveats: nested locks (keep inner lock events for deadlock), `cond_wait`
splits a section, read-write locks.

### 8.3 Races and deadlocks

- Races: filter candidates by lockset, create/join order, alias and atomics;
  then shadow flags (set on access, cleared after, assert no other thread
  holds one) on the remaining accesses, encoded as ordinary cells.
- Deadlocks: a lock-order graph with join edges first (no cycle means no
  deadlock). For cycles, add a stop point per thread: deadlock means some
  thread unfinished and all finished or blocked; `holds(u, L)` follows from
  `stop_u`'s position; an unfinished section ranks last for its lock;
  `cond_wait` is blocked if no later signal (the spurious-wakeup model is a
  choice to make).

### 8.4 All checks at once

Per-thread stop points (prefix semantics) for every check, which also stops
later assumes or locks in other threads from hiding a failing assertion.
Collapse a section only if it holds no race candidate and is not on a
deadlock cycle. Encode once; solve incrementally with one assumption literal
per property.

### 8.5 Still to plan

Memory safety across threads (alive state as a shared cell written by
malloc/free); the program ends at main's return or `exit()`, which kills
unjoined threads; `__VERIFIER_atomic_*` as one global lock; an unwinding
assertion on `pthread_create` in loops (UNKNOWN, not TRUE); violation
witnesses with thread waypoints; semaphores, barriers, `pthread_once`;
API-misuse checks (unlock not held, relock, destroy while locked, double or
detached join, thread ends holding a lock). Out of scope: liveness,
linearizability.

## 9. Milestones

### Milestone 0: memory spike (4–6 weeks)

Test the core bet before building the rest. The walker plus one flat byte
array, with no partitions, models or witnesses, on the 5–10 tasks where ESBMC
blows up: #9785's altera task, the list-model tasks, fuzzle, the large
drivers. Measure the walker's memory and the solver's memory separately.

ESBMC's SV-COMP runs use k-induction, so a blow-up in its results may come
from the inductive step, which this tool does not have yet. Before a task
enters the spike, rerun ESBMC on it in plain BMC (`--incremental-bmc`, same
flags and limits) and keep it only if ESBMC still blows up there. Features
outside the memory stage, such as function pointers, stay out: a task that
needs one is swapped for another, not served. The survey in
`docs/survey/2026-11-21-milestone-0-survey.md` records the candidates and what each needs.
Several of ESBMC's blow-ups live in the formula, not in unrolling (#10541's
144 MB formula, the list model's memcpy chains); if the solver dominates, the
walker alone does not fix them, and partitions and the memory encoding come
first.

Run SeaHorn (its SeaBMC engine is a bounded model checker on LLVM IR), SMACK
and Symbiotic (all LLVM-based, archives public) on the
same tasks. If they solve what ESBMC cannot, read how; if they fail too, their
failures show where byte memory or LLVM IR hurts. The gate below compares
with ESBMC, which is the wrong bar if another tool already wins there.

### Milestone 1

C only, one thread, ReachSafety, violation witnesses: the walker, byte memory
with partitions, the clang checks, camada, and the choices in §4.5.

Measure on SV-COMP Loops, Arrays, ControlFlow and Heap, plus the tasks where
ESBMC runs out of memory or time: #9785's altera task, the list-model
blow-ups, the large drivers.

**Decision gate:** milestone 1 must clearly beat ESBMC on memory and time on
those large tasks, with no wrong verdicts. If it does not, the core bet
(byte memory plus the walker) is wrong, and we learned it cheaply.

### After the gate

Vereda does not ship early inside ESBMC's SV-COMP entry: there is a year
before the next SV-COMP to build it as its own tool (decided 2026-11-23).
Stages 1 to 5 cover the SV-COMP categories; Test-Comp and C++ follow once
that work is done.

1. Memory safety and overflow.
2. k-induction and correctness witnesses.
3. Termination.
4. Validation, and counterexample replay: run the IR on the trace's
   nondet values with `lli` or the JIT; a mismatch is an encoder bug. The
   tool then reports FALSE only when the replay reaches the failing check,
   and UNKNOWN otherwise, as ESBMC's Python wrapper does. Moved out of
   milestone 1 on 2026-11-23.
5. Concurrency.
6. Test-Comp test generation.
7. C++.

## 10. Estimates

One experienced developer, full time, with agent help. Every number is
±50%; with half the time on ESBMC upkeep, double the calendar time.

Milestone 0 (§9): 4–6 weeks, most of it reused by the walker and encoder rows
below.

### Milestone 1

| Part | Weeks |
|---|---|
| Driver and clang setup (flags, pass list, linking the models) | 2–3 |
| Port ESBMC's C operational models, map the intrinsics | 2–4 |
| Walker | 4–6 |
| Instruction encoder on camada | 3–5 |
| Memory model | 6–10 |
| Sound field-sensitive points-to and partitioning (2–3 with SVF) | 4–8 |
| Folding as terms are built | 2–4 |
| Violation witnesses | 3–5 |
| benchexec tool-info, `test.desc` runner | 2–3 |
| Measuring and fixing up to the gate | 4–8 |
| **Total** | **32–56 (about 6–12 months)** |

Perhaps 15–25k lines of C++.

### After the gate

| Stage | Months |
|---|---|
| Memory safety and overflow | 3–5 |
| k-induction and correctness witnesses (incl. 1–2 for Crab) | 3–5 |
| Termination | 2–3 |
| Validation and counterexample replay | 2–4 |
| Concurrency (research: §8.1 design 2 is unmeasured) | 6–12 |
| C++ (library models, exceptions) | 6–12 |

Overall: about 2–2.5 years for the C categories without threads; 3–4 years
with concurrency and C++.

What could shorten it: camada, the models, the regression suite and the
sanitizers exist; agents write bulk code quickly; SVF instead of our own
points-to. What could lengthen it: the SV-COMP long tail (missing libc
models, broken benchmarks, validator quirks), a slow byte memory model, and
witness mapping through optimised IR.

## 11. Decisions to make during development

Each needs an explicit rule before the walker meets SV-COMP code.

- SMT meaning of `undef`, `poison` and `freeze`. Settle it before the
  encoder, since every instruction depends on it. Proposed default: an
  `undef` read is a fresh nondet value at each use (SV-COMP treats
  uninitialised locals as nondet); `freeze` makes it one fixed nondet value;
  with the no-wrap flags stripped, `poison` comes mostly from out-of-range
  shifts and similar, which the clang checks catch first, so it is also a
  nondet value, and a test pins each source that slips past the checks.
- Benchmark harness: each task in its own systemd scope with `RuntimeMaxSec`
  and `MemoryMax`, BenchExec for published numbers. On 2026-11-18 a batch
  under `timeout -s KILL 120` ran most jobs past 120 s and one for 3 days.
- Constructs: inline `asm` (common in LDV drivers), `va_arg` (lowered per
  ABI), `setjmp`/`longjmp`, `indirectbr` (computed goto), `x86_fp80` long
  double, `i128`, vector types, `cmpxchg`/`atomicrmw`/fences in
  single-threaded code, pointers in global constant initialisers,
  `exit`/`abort`/`atexit`.
- Solver policy: default solver, arrays as lemmas (Bitwuzla) or flattened,
  incremental and multi-property solving, portfolio, timeouts.
- Static points-to: our own or SVF.
- The spurious-wakeup model for `cond_wait`.
- Project basics: name, licence, repository, CI.

## 12. Risks

- Byte memory too slow even with partitions. Milestone 1 tests exactly this.
- Witness mapping through optimised IR; mitigated by the short pass list and
  debug info kept end to end.
- Unsound static analyses causing wrong TRUEs; mitigated by
  over-approximation rules, ESBMC as a second opinion, and counterexample
  replay (which catches wrong FALSEs).
- LLVM API churn; mitigated by pinning.
- Bus factor: the tool needs to beat ESBMC somewhere measurable early, or no
  one will keep it going.
