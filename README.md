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

`DOWNLOAD_DEPENDENCIES=ON` fetches the pinned LLVM 21 and camada releases into
`build/deps` (Linux x86_64 only) and builds with that LLVM's clang and lld,
since its libraries are LTO bitcode. With it off, set `LLVM_DIR` (LLVM 21 to
23) and `camada_DIR` to installed copies.

CMake keeps a build directory's first compiler. If you switch
`DOWNLOAD_DEPENDENCIES` on in an existing build directory, reconfigure with
`cmake --fresh` or the link fails with "file format not recognized".

## Run

```sh
./vereda.py --clang build/deps/llvm/LLVM-21.1.8-Linux-X64/bin/clang file.c --unwind 5
```

`vereda.py` takes C (`.c`, `.i`) or IR (`.ll`, `.bc`) inputs and its own
flags (`--clang`, `--bin`, `-m32`/`-m64`, `-I`, `-D`, `--keep-ir`,
`--property-file`, `-v`), and forwards everything else to the binary.
`vereda.py -h` lists both sets. The main verifier flags:

| Flag | Meaning |
|------|---------|
| `--unwind N` | Loop iterations and recursion depth (default: unroll until the next iteration is unreachable) |
| `--no-unwinding-assertions` | Assume the bound instead of checking it |
| `--incremental-bmc`, `--max-k-step N` | Try bounds 1, 2, ... until the verdict does not depend on the bound |
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
** Results:
loop_unwind_false.c, function main
  FAILED       [main.unwind.0]  line 10  unwinding assertion loop 0

** 1 of 1 properties failed

VERIFICATION FAILED
```

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
- `docs/spike-survey.md`: dated measurements and the design decisions made
  from them.

## License

Apache License 2.0; see `LICENSE`.
