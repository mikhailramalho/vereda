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
