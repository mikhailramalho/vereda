# Vereda

A bounded model checker for C that works on LLVM IR, with k-induction planned.
Vereda is Portuguese for a narrow trail: the walker follows every one through
the program.

## What it does

`vereda.py` compiles C to LLVM bitcode with clang, then the `vereda` binary
walks the IR from the entry function. As it walks, it unrolls loops and
recursion and encodes each instruction as SMT terms through
[camada](https://github.com/mikhailramalho/camada). The solver then either
finds a counterexample or proves that none exists within the bound.

Supported today:

- Properties: reachability of `reach_error`/`__VERIFIER_error`/`__assert_fail`,
  `__VERIFIER_assert`, plus unwinding and recursion assertions that check the
  bound was enough. With `--property-file`, one SV-COMP property: unreach-call,
  or no-overflow through clang's signed-overflow sanitizer.
- SV-COMP's `__VERIFIER_nondet_*` and `__VERIFIER_assume`, and `abort`/`exit`.
- Integers, pointers, structs and arrays in byte-addressed memory; `malloc`,
  `calloc`, `free`, `memcpy`, `memmove`, `memset`.
- Several C files, linked with llvm-link.
- Solvers: Bitwuzla (default), Z3, cvc5.

Anything else, such as floats, function pointers, threads or unmodelled
library calls, ends the run with `VERIFICATION UNKNOWN` and names what was
missing. Memory safety, overflow, k-induction, witnesses and concurrency are
planned (`docs/plan.md`, section 9).

## Build

Requires CMake 3.24 or later and the development packages for zlib, libxml2
and zstd (for LLVM) and GMP and MPFR (for camada's solvers), plus 32-bit libc
headers for `-m32` tests; on Debian/Ubuntu:
`zlib1g-dev libxml2-dev libzstd-dev libgmp-dev libmpfr-dev gcc-multilib`.

```sh
cmake -S . -B build -G Ninja -DDOWNLOAD_DEPENDENCIES=ON
ninja -C build
ctest --test-dir build
```

Vereda needs LLVM 23. `DOWNLOAD_DEPENDENCIES=ON` uses an installed LLVM 23 in
`/usr/lib/llvm-23`, as apt.llvm.org's `llvm-23-dev` and `clang-23` packages
install it. Without one, it fetches the pinned LLVM 23 release. It builds with
the LLVM's own clang, and its lld when there is one, unless you name a
compiler: the release's libraries are LTO bitcode, and the models' bitcode
must match the LLVM version. It fetches the camada release either way (Linux
x86_64 only), and mimalloc's sources, built into the binary in place of
glibc's malloc. Downloads go to `build/deps`, or to `$VEREDA_DEPS_DIR` when
set, which lets build directories and worktrees share one copy. A `LLVM_DIR`,
`camada_DIR` or `MIMALLOC_SOURCE_DIR` given on the command line is used
instead of a download; an empty `MIMALLOC_SOURCE_DIR` keeps glibc's malloc.
With it off, set `LLVM_DIR` (LLVM 23) and `camada_DIR` to installed
copies, and `MIMALLOC_SOURCE_DIR` to use mimalloc.

CMake keeps a build directory's first compiler. If you switch
`DOWNLOAD_DEPENDENCIES` on in an existing build directory, reconfigure with
`cmake --fresh` or the link fails with "file format not recognized". The same
goes for a build directory configured with an older LLVM, which now fails to
configure.

## Run

```sh
./vereda.py --clang build/deps/llvm/LLVM-23.1.6-Linux-X64/bin/clang file.c --unwind 5
```

The version banner comes before compilation and includes the executable's
build commit immediately after the camada version. Rebuilding refreshes the
short commit hash; builds without Git metadata report `commit unknown`.

`vereda.py` takes C (`.c`, `.i`) or IR (`.ll`, `.bc`) inputs and its own
flags (`--clang`, `--bin`, `-m32`/`-m64`, `-I`, `-D`, `--keep-ir`,
`--property-file`, `-v`, and ESBMC's `--overflow-check`,
`--unsigned-overflow-check` and `--ub-shift-check`, which compile in clang's
sanitizer checks), and forwards everything else to the binary.
`vereda.py -h` lists both sets. The main verifier flags:

| Flag | Meaning |
|------|---------|
| `--unwind N` | Loop iterations and recursion depth (default: unroll until the next iteration is unreachable) |
| `--no-unwinding-assertions` | Assume the bound instead of checking it |
| `--no-summarize-array-loops` | Unroll the counting loops that only write `a[i]`, check or count, which are otherwise replaced by their effect (ESBMC replaces those that only check or count only under `--summarize-array-loops`) |
| `--incremental-bmc`, `--max-k-step N` | Try bounds 1, 2, ... until the verdict does not depend on the bound |
| `--show-library-states` | Show Rust standard-library states in counterexamples |
| `--multi-property` | Check every property instead of stopping at the first violation |
| `--witness FILE` | Write the first counterexample that reaches `reach_error` as an SV-COMP violation witness (YAML 2.0); `-` is standard output |
| `--entry F` | Entry function (default `main`) |
| `--property unreach-call\|no-overflow` | Count only that property's failures and print `TRUE`, `FALSE_REACH`, `FALSE_OVERFLOW` or `UNKNOWN` after the verdict; `vereda.py --property-file` sets it |
| `--force-malloc-success` | `malloc` never returns null |
| `--solver bitwuzla\|z3\|cvc5` | SMT solver |
| `--dump-ir-only`, `--dump-ir-too` | Print the IR after the pass list, then stop or verify |
| `--dump-smt-only`, `--dump-smt-too` | Print the formula as SMT-LIB, then stop or solve |
| `--dump-dir D` | Write the dumps to files in `D` instead of printing them |

The run ends with a counterexample for each violation, a results list and
one of `VERIFICATION SUCCESSFUL` (exit 0), `VERIFICATION FAILED` (exit 1) or
`VERIFICATION UNKNOWN` (exit 2):

```
[RESULTS]
loop_unwind_false.c, function main
  FAILED       [main.unwind.0]  line 10  unwinding assertion loop 0

Properties: 1 checked, 0 passed, 1 failed, 0 not checked

VERIFICATION FAILED
```

Stage activity and loop unwinding use `[PROGRESS]`; target, timings and memory
use `[INFO]`; warnings use `[WARNING]`. A `[COUNTEREXAMPLE]` contains plain
assumption and state blocks, with binary values as inline comments,
followed by the violated property. `[RESULTS]` always appears for a verdict,
including successful runs with no remaining checks. Its counts describe
properties decided by the solver; checks resolved during the walk do not
appear in that table. An UNKNOWN verdict gives its reason instead of counts.
Property IDs, exit codes and the final `VERIFICATION ...` and SV-COMP verdict
lines are unchanged.

A run that ends in an error instead of a verdict (a bad option, an input that
cannot be loaded, a failed compile) exits with 6, as in ESBMC.

## Tests

A test is a C or IR file under `tests/<suite>/`, or
`tests/<suite>/<name>/main.c` for a test with several files. Its comment
header says how to run it and what to expect:

```c
// RUN: --unwind 5
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: VERIFICATION SUCCESSFUL
// MODE: FUTURE          (optional: CORE, THOROUGH, KNOWNBUG, FUTURE)
// FILES: helper.c       (optional: extra sources next to the test)
```

IR tests use `;` instead of `//`. `ctest -L <suite>` runs one suite, and
`ctest -R <suite>/<name>` runs one test. KNOWNBUG and FUTURE tests must fail;
a run that passes asks to be promoted to CORE.

`tests/import_esbmc.py <esbmc>/regression [suite ...]` copies ESBMC's C
tests into this format, translating the flags Vereda has. Tests that need any
other flag become FUTURE.

## Documentation

- `docs/plan.md`: goals, design and milestones.
- `docs/architecture.md`: the stages under `src/`, their interfaces and
  layering rules.
- `docs/survey/`: dated measurements and the design decisions made from
  them, one file per entry.

## License

Apache License 2.0; see `LICENSE`.
