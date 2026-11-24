# Architecture

One folder per stage, in the order the data flows. Each stage has one job and
one interface to the next. Folders not yet present are created when their
stage is started.

```
vereda/
├── vereda.py            front-end driver: clang, lli replay
├── models/              C library models, built to .bc; ir links what a program calls
├── src/
│   ├── ir/              load bitcode, pass list, strip UB flags, reject mustprogress,
│   │                    make irreducible CFGs loops, debug-info maps. The only place
│   │                    that tracks LLVM API churn.
│   ├── analysis/        static pre-analyses on the IR: points-to, partitions,
│   │                    shared cells, value flow, slicing; later Crab invariants
│   ├── walker/          symbolic execution: RPO walk, guards, unrolling, call
│   │                    contexts, the loop transition relation, k-induction,
│   │                    incremental BMC
│   ├── memory/          (object, offset) pointers, object table, byte arrays per
│   │                    partition, load/store encoding
│   ├── encode/          instruction → camada terms, folding (terms.h, which
│   │                    memory also builds with), nondet tagging
│   ├── checks/          property checks: reachability, deref, free, overflow,
│   │                    memcleanup, termination. Each check tagged with property
│   │                    and location.
│   ├── solver/          camada glue: assumption literal per check, incremental
│   │                    solving, timeouts, solver choice
│   ├── witness/         trace from the model, violation and correctness witnesses,
│   │                    validation mode
│   ├── util/            options, diagnostics, logging
│   └── driver/          main.cpp: parse options, run the stages, print the verdict
├── tests/               tests by suite; each test is a source file whose comment
│                    header gives flags and expected output (tests/run.py)
├── scripts/             benchexec tool-info, SV-COMP wrapper, release
└── docs/                the plan and this file
```

## Data flow

```
C ──vereda.py──> clang ──> .bc ──> ir ──> analysis ──> walker ──> solver ──> verdict
                                            │            │ ▲                   │
                                            │     encode + memory + checks     ▼
                                            └──────────────────────────> witness
```

The walker is the hub. It drives encode, memory and checks per instruction as
it walks: unroll, points-to and encode in one pass (plan, section 4.2).
Everything to its left is whole-program and runs once; everything to its right
is per-term.

## Seams

These interfaces are what later stages build on, so they are fixed first.

- `ir` produces a Program: the module plus loop info, dominator trees and debug
  maps per function. Nothing after `ir` reads a pass or an attribute.
- `analysis` answers two queries: pts(value) as a set of abstract objects, and
  partition(object). Over-approximate by rule; a miss here is a wrong TRUE.
- `walker` to `encode` and `memory`: encode(inst, env) returns a term;
  load(guard, pointer, width) and store(guard, pointer, term) go through
  `memory`, which owns the partition arrays and the per-step points-to sets.
- `checks` collects Check{property, location, term}. The verdict counts only
  failed checks of the property asked for.
- `solver` owns the one incremental solver instance and the assumption
  literals.
- `witness` consumes a Trace: ordered steps with guard, location, branch mark
  and tagged nondet values. It never looks at the walker's state.

## Rules

- A stage includes only stages to its left. `witness` reads `solver` and `ir`,
  never `walker` internals.
- LLVM headers are allowed in `ir`, `analysis`, `walker` and `encode`, since
  they consume instructions. API churn that bites at upgrades (passes,
  attributes, debug records, metadata) stays in `ir`.
- camada headers are allowed in `encode`, `memory`, `checks` and `solver` only.
- `src/*` builds one static library, `libvereda`, and `driver` links it. Unit
  tests link the library per stage.

## Differences from ESBMC

- No separate GOTO stage. LLVM IR after `ir` is the program representation, so
  `ir` plus `analysis` replace goto-programs and pointer-analysis.
- `memory` is its own stage. In ESBMC it is spread over dereference, value sets
  and the SMT backend, which is where the typed-memory bugs lived.
- `checks` is a stage, not instrumentation on the program. Checks are terms
  added during the walk, tagged, and selected at solve time, so one walk
  serves every property.
- Concurrency later gets a `concurrency/` folder between `walker` and
  `encode`, holding the partial-order encoding and the lockset collapse from
  plan section 8.
