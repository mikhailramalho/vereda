# Vereda

A bounded model checker for C and Rust that works on LLVM IR, with k-induction planned.
Vereda is Portuguese for a narrow trail: the walker follows every one through
the program.

## What it does

`vereda.py` compiles C with clang or Rust with rustc to LLVM bitcode, then the `vereda` binary
walks the IR from the entry function. As it walks, it unrolls loops and
recursion and encodes each instruction as SMT terms through
[camada](https://github.com/mikhailramalho/camada). The solver then either
finds a counterexample or proves that none exists within the bound.

Supported today:

- Properties: reachability of `reach_error`/`__VERIFIER_error`/`__assert_fail`,
  `__VERIFIER_assert`, plus unwinding and recursion assertions that check the
  bound was enough. With `--property-file`, one SV-COMP property: unreach-call,
  no-overflow through clang's signed-overflow sanitizer, or termination, shown
  when SCEV bounds every loop and nothing recurses, or when the bound unwinds
  every loop and recursion (never FALSE).
- SV-COMP's `__VERIFIER_nondet_*` and `__VERIFIER_assume`, and `abort`/`exit`.
- Stack allocations and VLAs above the target's `PTRDIFF_MAX` are reported as
  `alloca: size exceeds PTRDIFF_MAX`; exactly the limit remains valid.
- Integers, pointers, structs and arrays in byte-addressed memory; `malloc`,
  `calloc`, `free`, `memcpy`, `memmove`, `memset`.
- Several C files, linked with llvm-link.
- Solvers: Bitwuzla (default), Z3, cvc5.

Anything else, such as floats, function pointers, threads or unmodelled
library calls, ends the run with `VERIFICATION UNKNOWN` and names what was
missing. General memory safety, k-induction and concurrency checks are
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

Rust uses rustc 1.100.0, whose bitcode matches LLVM 23. CMake uses `rustc`
from PATH when it has that version, or `-DRUSTC_EXECUTABLE=/path/to/rustc`.
With `DOWNLOAD_DEPENDENCIES=ON`, a missing or different system version makes
it download the pinned compiler and standard library from static.rust-lang.org,
verify their SHA-256 hashes, and install them under the dependency directory.
This does not change rustup or install Cargo. Without downloads, supply rustc
1.100.0. The selected path is saved beside the binary for the front end.

CMake keeps a build directory's first compiler. If you switch
`DOWNLOAD_DEPENDENCIES` on in an existing build directory, reconfigure with
`cmake --fresh` or the link fails with "file format not recognized". The same
goes for a build directory configured with an older LLVM, which now fails to
configure.

## Run

```sh
./vereda.py file.c --unwind 5
./vereda.py file.rs --unwind 5
```

The C compiler defaults to `CLANG`, then the LLVM compiler recorded by CMake,
then `clang` on PATH. Use `--clang <path>` to override it. Its major version must
match the verifier's LLVM version; multi-file inputs use its sibling `llvm-link`.

The version banner comes before compilation and includes the executable's
build commit immediately after the camada version. Rebuilding refreshes the
short commit hash; builds without Git metadata report `commit unknown`.

`vereda.py` takes C (`.c`, `.i`), Rust (`.rs`) or IR (`.ll`, `.bc`) inputs and its own
flags (`--clang`, `--rustc`, `--bin`, `-m32`/`-m64`, `-I`, `-D`, `--keep-ir`,
`--property-file`, `-v`, and the checks it compiles in with clang's
sanitizers: `--ub-signed-overflow-check`, `--unsigned-overflow-check`,
`--ub-shift-check`, and `--overflow-check` for both overflow checks), and
forwards everything else to the binary. Each check flag checks one thing and
takes `=true` or `=false`; the last one naming a check wins. Unsigned
arithmetic wraps by definition (C11 6.2.5p9), so its check has no `ub-`
prefix. Unlike ESBMC's `--overflow-check`, which checks signed overflow,
shifts and floating-point to integer conversions, Vereda's checks signed and
unsigned overflow only; `tests/import_esbmc.py` translates ESBMC's flags.
`vereda.py -h` lists both sets.

Rust source is compiled with edition 2024, debug info, no optimisation,
checked integer arithmetic and `panic=abort`. Ordinary `fn main()` and
`#![no_main]` programs with a C entry point are accepted; verification enters
the source main directly, bypassing Rust runtime startup. `--keep-ir` also
works for Rust. `--rustc` (or `$RUSTC`) overrides CMake's compiler, and must
still be version 1.100.0. Rust source currently targets the 64-bit host;
`-m32`, C sanitizer flags, C property files, `-I` and `-D` are rejected.
Cargo projects and external Rust crates are not built by this driver.

The main verifier flags:

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
| `--property unreach-call\|no-overflow\|valid-free\|termination` | Count only that property's failures and print `TRUE`, `FALSE_REACH`, `FALSE_OVERFLOW`, `FALSE_FREE` or `UNKNOWN` after the verdict; `vereda.py --property-file` sets unreach-call, no-overflow and termination |
| `--force-malloc-success` | `malloc` never returns null |
| `--no-assertions` | Disable user assertion checks; unwinding and recursion checks remain enabled |
| `--ub-pointer-check` | Check writes into string literals and const objects, and invalid frees (default on; `=false` disables) |
| `--ub-div-by-zero-check` | Check integer division/remainder by zero (default on for C and IR inputs unless `--property` is given; `=false` disables) |
| `--ub-count-zero-check` | Check undefined zero arguments to `clz`/`ctz` builtins and LLVM count intrinsics; defined-zero variants remain valid (opt-in) |
| `--ub-float-cast-overflow-check` | Check out-of-range floating-point to integer conversions (opt-in) |
| `--ub-alignment-check` | Check alignment of typed memory accesses (opt-in) |
| `--ub-uninit-check` | Check reads of uninitialised local values and memory (opt-in) |
| `--memory-leak-check` | Request leak checking; currently errors because it is not implemented |
| `--ub-data-race-check` | Request data-race checking; currently errors because it is not implemented |
| `--deadlock-check` | Request deadlock checking; currently errors because it is not implemented |
| `--vla-size-check` | Report stack allocations above `PTRDIFF_MAX`, an implementation limit rather than undefined behaviour (default on; `=false` disables; their paths still end at the allocation) |
| `--solver bitwuzla\|z3\|cvc5` | SMT solver |
| `--incremental-solver` | Keep one incremental solver across checks (with `bv-cnf-scope=permanent` on Bitwuzla) instead of fresh non-incremental solves |
| `--dump-ir-only`, `--dump-ir-too` | Print the IR after the pass list, then stop or verify |
| `--dump-smt-only`, `--dump-smt-too` | Print the formula as SMT-LIB, then stop or solve |
| `--dump-dir D` | Write the dumps to files in `D` instead of printing them |

Assertions, the current pointer checks, integer division/remainder-by-zero checks,
and unwinding checks are enabled by default. The pointer checks include ESBMC's
checks on `free` and `realloc`: a pointer made from an integer, one into a
non-heap object or into the middle of a heap object, and a second free. General
pointer bounds and lifetime checks are not implemented yet. Signed overflow, unsigned overflow
and shift checks are opt-in through the frontend flags above; existing Rust
overflow and bounds panics remain program failures. Leak, race and deadlock flags
are disabled by default and the binary rejects an enabled request with error
exit code 6, before loading the input. `--no-assertions` conflicts with an
unreach-call property request, `--ub-pointer-check=false` with valid-free, `--no-unwinding-assertions` with termination. A selected property reports only its own failures.
An `[INFO] Checks:` line before the walk names the checks the run counts,
each after the flag that controls it: `assertions`, `unwinding-assertions`,
`pointer`, `div-by-zero` and `vla-size` by default, then the opt-in ones given
(`signed-overflow`, `unsigned-overflow`, `shift`, `float-cast-overflow`,
`count-zero`, `alignment`, `uninit`); under `--property`, `reach-error` or
`valid-free` and the property's own checks.

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
