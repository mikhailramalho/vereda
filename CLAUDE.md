# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

Vereda is a bounded model checker (k-induction planned) for C, working on LLVM IR. It is
built next to ESBMC (not as a replacement), so its flags, output and property ids follow
ESBMC's.
`docs/plan.md` has the goals and milestones, `docs/architecture.md` the stage layout and
its rules, `docs/spike-survey.md` a dated log of measurements and design decisions.

## Build, run, test

```sh
cmake -S . -B build -G Ninja -DDOWNLOAD_DEPENDENCIES=ON   # pinned LLVM 21 + camada into build/deps
cmake --build build                                       # the existing build/ may use Makefiles
ctest --test-dir build -L vereda -j8                      # Vereda's own suite (fast; the suite dir is the label)
ctest --test-dir build -j$(nproc)                         # everything, incl. ~6,500 imported esbmc* tests
ctest --test-dir build -R 'vereda/loop_nested$' --output-on-failure   # one test
```

Without `DOWNLOAD_DEPENDENCIES`, set `LLVM_DIR` (LLVM 21 to 23) and `camada_DIR`.
The downloaded LLVM is an LTO build, so its own clang/lld become the compiler and linker.

Run the verifier through the front end, which compiles C with clang (and llvm-link for
several files) and forwards every flag it does not know to the binary:

```sh
./vereda.py --clang build/deps/llvm/LLVM-21.1.8-Linux-X64/bin/clang file.c --unwind 5
./vereda.py -h                      # front-end flags, then the binary's
```

The binary (`build/vereda`) takes only `.bc`/`.ll`; its flags are `cl::opt`s in
`src/driver/main.cpp`. Useful for debugging: `--dump-ir-only`, `--dump-smt-only`,
`--dump-dir <dir>`, `--keep-ir` (front end). Exit codes: 0 SUCCESSFUL, 1 FAILED or error,
2 UNKNOWN.

## Regression tests

A test is a `.c` or `.ll` file directly under `tests/<suite>/`, or
`tests/<suite>/<name>/main.c` for multi-file tests. Its comment header (`;` in IR)
drives `tests/run.py`:

```c
// RUN: --unwind 5                 flags for vereda.py
// EXPECT: ^VERIFICATION FAILED$   regex, re.MULTILINE, must match
// EXPECT-NOT: VERIFICATION SUCCESSFUL
// MODE: KNOWNBUG                  CORE (default), THOROUGH, KNOWNBUG, FUTURE
// FILES: helper.c                 extra sources next to the test
```

KNOWNBUG and FUTURE tests show as skipped while some expectation fails; when one starts
passing, promote it to CORE. New test files need a cmake re-configure to be picked up
(the glob uses CONFIGURE_DEPENDS, so `cmake --build` usually handles it).
`tests/import_esbmc.py <esbmc>/regression [suite ...]` imports ESBMC's C tests,
translating flags; tests needing flags Vereda lacks become FUTURE with a `// Needs:` line,
and `--mark` flips the mode of tests Vereda currently disagrees with. Every test has a 300 s
timeout (`REGRESS_TIMEOUT`); a run that times out is skipped in any mode, and `--mark`
never changes a mode because of one.
Every test also runs under a memory limit (`REGRESS_MEMORY_LIMIT`, 4096 MB of address
space); going past it fails the test, except a KNOWNBUG one, and `--mark` leaves its mode
alone. CI runs 3 tests at a time so that three such tests fit its 16 GB.
When adding a flag that has an ESBMC equivalent, add it to the tables in that script.

## Architecture

One directory per stage under `src/`, all built into one static library `libvereda`;
`src/driver/main.cpp` links it and runs the stages. Data flows
`vereda.py → clang → ir → walker → solver → verdict/witness`, with the walker as the hub:
in one pass over the rolled IR it unrolls loops on the fly, walks calls in their own
frame, and drives `encode` (instruction → camada terms), `memory` (object/offset
pointers, byte arrays, known-byte map) and `checks` per instruction.

- `ir`: loads bitcode, runs the pass list, rejects `mustprogress` and irreducible CFGs,
  lowers indirect calls to tests and direct calls (as CBMC's remove_function_pointers),
  turns bodyless libm calls (sqrt, floor, ...) into LLVM intrinsics, scalarizes vectors,
  then links the C library models (`models/*.c`, built to `build/models-{32,64}.bc`)
  for the functions the program still calls without defining,
  builds debug-info maps. The only place that tracks LLVM API churn.
- `encode/terms.*`: the term builder that hash-conses and folds as terms are built;
  `memory` builds through it too. A guard that folds to false prunes the branch.
- `checks`: each `Check` is a term satisfiable iff the property is violated, tagged with
  property and location; `id()` gives ESBMC's id (`main.assertion.1`, `main.unwind.0`).
  One walk serves all checks; the solver selects them via assumption literals.
- `witness`: consumes the `walker::Trace` and the solver model, never walker internals.
- IR a stage cannot handle throws `vereda::Unsupported` (`src/util/unsupported.h`) or
  returns an `llvm::Error`; the driver turns either into `VERIFICATION UNKNOWN`.

Layering rules from `docs/architecture.md`: a stage includes only stages to its left;
LLVM headers only in `ir`, `analysis`, `walker`, `encode`; camada headers only in
`encode`, `memory`, `checks`, `solver`. Directories listed there but absent (`analysis/`,
`models/`, `scripts/`) are created when that stage starts.

## Conventions

- Each header opens with a `// Stage <name>: ...` comment saying what it does.
- Comments are plain prose; don't add `ponytail:` tags (they were removed on purpose).
- Commit messages: short imperative subject, then a prose body explaining why and what
  changed in behaviour, no bullet lists.
- Record measurements and design decisions (including ones not adopted) as a dated
  section in `docs/spike-survey.md`.
