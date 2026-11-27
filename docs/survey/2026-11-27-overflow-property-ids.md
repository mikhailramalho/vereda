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

Each probe ran with `--multi-property` on ESBMC 8.8.0 and Vereda, with
ESBMC's flags (Vereda before this change needed its own flag where noted).
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

The flags follow ESBMC's too. `--overflow-check` and
`--unsigned-overflow-check` now also check shifts, as ESBMC's
`goto_checkt::shift_check` does under either, and `--overflow-check` also
checks float-to-integer conversions (`fp_to_int_range_check`) unless
`--no-fp-conversion-check`, a new front-end flag, is given. The binary takes
ESBMC's `--clz-zero-check` and `--uninitialised-vars-check` as aliases of
`--ub-count-zero-check` and `--ub-uninit-check`. `tests/import_esbmc.py`
passes those two and `--no-fp-conversion-check` through, and drops
`--no-fp-overflow-check` (Vereda has no floating-point arithmetic overflow
check to turn off).

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

`--ub-float-cast-overflow-check` stays as the binary's flag, since raw IR has
no `--overflow-check` front end; ESBMC has no separate flag.

ESBMC's `--ub-shift-check` alone also turns on its signed add, sub and mul
checks (its `overflow_check` returns early only when none of the three flags
is given). Vereda's checks shifts only: matching that would make a shift
flag check additions.

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
these messages all pass. Seventeen tests whose Needs line named the newly
accepted flags now run: nine of the fourteen uninit tests,
`github_10173_fp_overflow_off` and its `_fail`, and `github_7572_nocheck`
became CORE; five uninit tests stay FUTURE for ESBMC's `CWE:` line or its
array cap. The new shift and float-cast checks promoted `github_2789`,
`github_578_fail4`, `github_578_fail5`, `github_7572_fail`,
`github_7572_trunc_fail`, `github_9223_fail` and `github_9436_fail` to CORE;
`github_381` now gives ESBMC's verdict but stays FUTURE for the column.

Follow-up issues: #349, #350, #351.
