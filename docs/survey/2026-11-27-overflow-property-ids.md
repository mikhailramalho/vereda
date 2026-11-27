# Overflow and undefined-behaviour property ids against ESBMC 8.8

Recorded 2026-11-27.

Issue #312 asked for Vereda's C overflow checks to report the property ids,
kinds and messages ESBMC reports for the same program. The newer checks from
#257, #288, #289 and #290 (division by zero, float-to-integer conversion,
alignment, uninitialised reads) had ids of their own that ESBMC 8.8 never
prints.

## How ESBMC 8.8 names a property

ESBMC builds an id as `<function>.<class>.<n>`. The class comes from the
check's comment through the substring table in `src/util/base/cwe_mapping.cpp`
(`arithmetic overflow` gives `arithmetic-overflow`, `undefined behavior on
shift operation` gives `shift-undefined-behavior`, `use of uninitialized
variable` gives `uninitialised-variable`, `division by zero` gives
`division-by-zero`); a comment the table lacks gives `assertion`. The numbers
count per function and class in the order of the results table, which
`property_report.cpp` sorts by file, function, line, column and comment, so
they follow the source, not the order symex met the checks.

Vereda numbered each check in walk order per function and `Property`, and
filed every clang sanitizer trap under `assertion`. Both now follow ESBMC:
`checks::number` numbers the checks after the walk in ESBMC's order, the
driver prints the table in that order, and each `Property` maps to ESBMC's
class.

## Table

Each probe ran with `--multi-property` on ESBMC 8.8.0 with the flag shown,
and on Vereda with the flags `tests/import_esbmc.py` translates it to (see
Flags below; Vereda before this change needed its own flag where noted).
One entry per row, `id message`.

| Program | Flag | Vereda before | Vereda after | ESBMC 8.8 |
|---|---|---|---|---|
| `x + 1`, `x++`, `x += 5` (int) | `--overflow-check` | `main.assertion.1` arithmetic overflow on add | `main.arithmetic-overflow.1` arithmetic overflow on add | as after |
| `x - 1`, `x * 2`, `-x`, `long long * 3` | `--overflow-check` | `main.assertion.1` arithmetic overflow on sub/mul/neg | `main.arithmetic-overflow.1` same messages | as after |
| `x / y`, `INT_MIN / -1` | `--overflow-check` | `main.assertion.1` division by zero or overflow | `main.arithmetic-overflow.1` arithmetic overflow on div | as after (plus a passing `division-by-zero.1`) |
| `x % y`, `INT_MIN % -1` | `--overflow-check` | `main.assertion.1` division by zero or overflow | `main.arithmetic-overflow.1` arithmetic overflow on modulus | as after |
| unsigned `x + 1u`, `x - 1u`, `x * 3u` | `--unsigned-overflow-check` | `main.assertion.1` arithmetic overflow on add/sub/mul | `main.arithmetic-overflow.1` same messages | as after |
| `x << 4`, `x >= 0` | `--ub-shift-check` or `--overflow-check` | `main.assertion.1` shift out of bounds; not checked under `--overflow-check` | `main.arithmetic-overflow.1` arithmetic overflow on shl | as after (plus a passing shift check) |
| `1 << s`, `s < 0` | `--ub-shift-check` or `--overflow-check` | `main.assertion.1` shift out of bounds | `main.arithmetic-overflow.1` arithmetic overflow on shl, `main.shift-undefined-behavior.1` undefined behavior on shift operation shl | as after |
| `x >> s` | `--ub-shift-check` or `--overflow-check` | `main.assertion.1` shift out of bounds | `main.shift-undefined-behavior.1` undefined behavior on shift operation ashr | as after |
| `(int)d`, `d` a nondet double | `--overflow-check` | not checked; with `--ub-float-cast-overflow-check` `main.float-cast-overflow.1` float cast overflow | `main.assertion.1` floating-point conversion out of range of signedbv on typecast | as after |
| `__builtin_clz(x)`, `__builtin_ctz(x)` | `--clz-zero-check` | with `--ub-count-zero-check` `main.count-zero.1` __builtin_clz/ctz of zero is undefined | `main.assertion.1` same message | as after |
| `*(int *)(p + 1)`, `p` from malloc | ESBMC: default; Vereda: `--ub-alignment-check` | `main.alignment.1` dereference failure: Incorrect alignment when accessing data object | `main.assertion.1` same message | as after |
| `int x; return x;` | `--uninitialised-vars-check` | with `--ub-uninit-check` `main.uninitialised-read.1` read of uninitialised memory | `main.uninitialised-variable.1` use of uninitialized variable: x | as after |
| `*p`, `p = malloc(4)` never written | `--uninitialised-vars-check` | with `--ub-uninit-check` `main.uninitialised-read.1` read of uninitialised memory | `main.uninitialised-variable.1` read of uninitialised memory | not checked (passes) |
| `x / y`, `y` may be 0 | none | `main.division-by-zero.1` division by zero | unchanged | as Vereda |
| `(int)l`, `l` a long; `a + i` on a pointer | `--overflow-check` | not checked | not checked | not checked |
| an assertion, add, mul, div, assertion, sub, add on lines 6 to 12 | `--overflow-check` | `main.assertion.1` to `.7` in walk order (line 9 first, line 6 last) | `main.assertion.1`, `.2` (lines 6, 10) and `main.arithmetic-overflow.1` to `.5` (lines 7, 8, 9, 11, 12) | same ids |

## What changed

The ids come from `Check::kind()`: `arithmetic-overflow`, the new `Shift`
property's `shift-undefined-behavior`, `division-by-zero`,
`uninitialised-variable` (was `uninitialised-read`), `array-bounds-violated`,
and `assertion` for assertions and for the count-zero, float-cast and
alignment checks, whose comments ESBMC's table does not name. Those three
keep their own `Property`, so `--no-assertions` still leaves them on; only
their printed class and numbering are shared with assertions, as in ESBMC.

A clang trap tells its kind of check, not the operation: one code covers
signed division and remainder, another every shift. Before InstCombine moves
or merges anything, `nameSanitizerChecks` in `ir/load.cpp` finds the
operation each division, shift and float-cast check guards and attaches
ESBMC's comment as `vereda.check` metadata, which the walker reads (with the
old generic text as fallback). Clang checks a signed `<<` exponent and base in
one branch; ESBMC calls a non-negative base that loses bits an arithmetic
overflow and everything else (a bad exponent or a negative base) undefined
behaviour, so that trap is split in two on `exponent ok && base >= 0`, the
overflow trap walked first so that a run stopping at the first failure
reports it first, as ESBMC does. A
float-cast check names the destination as ESBMC does, `signedbv` or
`unsignedbv`.

## Flags

Vereda names its checks its own way, and ESBMC compatibility lives only in
the importer's tables. Every undefined-behaviour check has a `--ub-*` flag
that checks one thing; a check on by default is turned off with `=false`
(LLVM's `cl::opt<bool>` parses it, and `vereda.py` parses its own flags the
same way, the last one naming a check winning), not with a `--no-*` flag.
A check that is not undefined behaviour has no `ub-` prefix.

| Flag | Checks | Default |
|---|---|---|
| `--ub-signed-overflow-check` | signed integer overflow (C11 6.5p5), clang's `signed-integer-overflow` (front end) | off |
| `--unsigned-overflow-check` | unsigned wrap-around, clang's `unsigned-integer-overflow` (front end); not UB, since unsigned arithmetic wraps (C11 6.2.5p9) | off |
| `--overflow-check` | shorthand for both of the above (front end) | off |
| `--ub-shift-check` | shifts, clang's `shift` (front end) | off |
| `--ub-float-cast-overflow-check` | out-of-range floating-point to integer conversions | off |
| `--ub-div-by-zero-check` | integer division and remainder by zero | on |
| `--ub-count-zero-check` | zero arguments to `clz`/`ctz` | off |
| `--ub-alignment-check` | alignment of typed accesses | off |
| `--ub-uninit-check` | reads of uninitialised locals and memory | off |
| `--ub-pointer-check` | writes into string literals and const objects | on |
| `--vla-size-check` | stack allocations above `PTRDIFF_MAX`, an implementation limit rather than UB (only a VLA size of zero or less is, C11 6.7.6.2p5) | on |
| `--ub-data-race-check` | data races (C11 5.1.2.4p25); not implemented, errors | off |
| `--memory-leak-check` | leaks, not UB; not implemented, errors | off |

These replace `--no-div-by-zero-check`, `--no-pointer-check`,
`--no-vla-size-check` and `--data-race-check` (whose `--data-races-check`
alias is gone too). The binary has no ESBMC aliases: `--clz-zero-check` and
`--uninitialised-vars-check` are not accepted, and the front end has no
`--no-fp-conversion-check`.

Vereda's `--overflow-check` differs from ESBMC's. ESBMC's `goto_checkt`
checks a signed operation under any of `--overflow-check`,
`--unsigned-overflow-check` and `--ub-shift-check` and an unsigned one only
under `--unsigned-overflow-check` (`overflow_check`), checks shifts under
any of the three (`shift_check`), and checks floating-point to integer
conversions under `--overflow-check` unless `--no-fp-conversion-check` is
given (`fp_to_int_range_check`); an unsigned run of ESBMC on
`int a; a + (x >> s)` reports `arithmetic overflow on add` for the signed
add. Vereda's flags each check one of these, so `tests/import_esbmc.py`
translates ESBMC's:

| ESBMC | Vereda |
|---|---|
| `--overflow-check` (or `--overflow`) | `--ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check` |
| `--overflow-check --no-fp-conversion-check` | `--ub-signed-overflow-check --ub-shift-check` |
| `--unsigned-overflow-check` | `--ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check` |
| `--ub-shift-check` | `--ub-signed-overflow-check --ub-shift-check` |
| `--clz-zero-check` | `--ub-count-zero-check` |
| `--uninitialised-vars-check` | `--ub-uninit-check` |
| `--no-div-by-zero-check` | `--ub-div-by-zero-check=false` |
| `--no-pointer-check` | `--ub-pointer-check=false` |
| `--no-vla-size-check` | `--vla-size-check=false` |
| `--data-races-check` | `--ub-data-race-check` (unsupported, so the test stays FUTURE) |
| `--no-fp-overflow-check` | dropped: Vereda has no floating-point arithmetic overflow check |

Combined flags give the union, each Vereda flag once. The imported tests'
RUN lines were rewritten with these tables in place, keeping their modes
and recorded verdicts, rather than re-imported: the ESBMC checkout has moved
on since the last import (274 imported tests have no test.desc there now).

The SV-COMP no-overflow property still turns on the signed check only, and
Rust keeps rustc's own overflow checks.

## Kept as Vereda's, and why

`--ub-alignment-check` stays opt-in; ESBMC checks alignment by default. Making
it default would cost every typed access a check and expose locals' addresses
(#289's survey), for a property few programs violate; ESBMC's
`--no-align-check` is already dropped by the importer.

`--ub-uninit-check` also checks heap and pointer reads, which ESBMC does not;
those keep the message "read of uninitialised memory" under the
`uninitialised-variable` class. A read of a named local or array names it as
ESBMC does. ESBMC also skips arrays above 4096 elements, which Vereda checks
(`esbmc/cwe_uninit_array_oversize` stays FUTURE).

## What still differs

Clang folds a check InstCombine proves redundant (a second shift by the same
amount after a first one checked it), so Vereda lists fewer properties than
ESBMC, which keeps them as passing rows. Numbers of the remaining checks of a
kind can then differ from ESBMC's.

A failed sanitizer check ends its path in Vereda, so under
`--multi-property` a later check is only violated on paths that pass the
earlier ones; ESBMC continues past a failed check, and lists more failures.

ESBMC's "Violated property" block names the column of the statement and
adds the check's condition (`!overflow("+", x, 1)`) and a `CWE:` line;
Vereda names the operator's column and prints neither. ESBMC also checks `abs(INT_MIN)` ("arithmetic overflow on abs"), which
Vereda does not.

Outside this issue: ESBMC 8.8 prints an unwinding assertion as
`main.assertion.<n>` ("unwinding assertion loop 3") and a recursion bound as
`f.assertion.<n>`, where Vereda prints `main.unwind.<loop>` and
`f.recursion.<n>`; Vereda's own tests encode those ids.

## Tests

The Vereda suite passes (633 tests, the existing `lib_sprintf_overflow`
skip). `vereda/assume_range_summary_fail` now numbers its assertions by line,
and the UB check scripts (`count_zeros.py`, `float_cast_overflow.py`,
`alignment_check.py`, `uninit_check.py`, `check_flags.py`,
`output_readers.py`) expect the new ids and messages. Of the imported tests,
the 726 that use an overflow, shift, count-zero, alignment, uninit or
fp-conversion flag, `--multi-property`, or expect a property id or one of
these messages all pass. Seventeen tests whose Needs line named ESBMC
flags the importer now translates run: nine of the fourteen uninit tests,
`github_10173_fp_overflow_off` and its `_fail`, and `github_7572_nocheck`
became CORE; five uninit tests stay FUTURE for ESBMC's `CWE:` line or its
array cap. The new shift and float-cast checks promoted `github_2789`,
`github_578_fail4`, `github_578_fail5`, `github_7572_fail`,
`github_7572_trunc_fail`, `github_9223_fail` and `github_9436_fail` to CORE;
`github_381` now gives ESBMC's verdict but stays FUTURE for the column.

With Vereda's own flags, `check_flags.py` checks that each overflow flag
checks only its own operations, that `--overflow-check` is both and
`=false` turns one back off, and that a bad value is an error;
`output_readers.py` checks the importer's translations, and
`frontend_options.py` that Rust input rejects the C check flags. The RUN
lines of 879 imported tests changed (788 for the check flags, 91 for
`--ub-data-race-check`). The 1185 imported tests that changed or use an
overflow, shift, count-zero, alignment, uninit, float-cast or other `--ub-*`
flag or `--multi-property`, or expect one of these ids or messages, all pass
(319 run, 866 FUTURE or KNOWNBUG with their recorded verdicts); no verdict
changed, including the 13 ESBMC `--ub-shift-check` tests that now also check
signed overflow.

Follow-up issues: #349, #350, #351, #385, #386.
