# Symbolic offsets in integer addresses (#226)

Compared with main at 7a9aaa11, using LLVM 23.1.6 and camada 0.25's
Bitwuzla 0.11.0. The preceding experiment is
`2026-11-25-int-base-stores.md`.

## Recovering the object

`Memory::fromAddress` still tests `exists && address - base <= size`.
The encoder now supplies a lookup of its current path's bounds, and memory
asks `Terms::decide` about the offset comparison. The answer is used only
for this conversion: no path-dependent rewrite enters Terms' global caches.
An object's existence must still hold, either unconditionally or under the
current guard, before returning its object directly. An unknown comparison
keeps the original choice, including the outside case.

LLVM represents multiplication by a power of two as a left shift. Range
analysis now handles constant left shifts, declining a range when its
unsigned endpoints would wrap. Without that, even `base + 4*k` under
`k < 4` still retained the outside case.

For `ite(c, base_a, base_b) + offset`, distribute the additions to recover
each arm separately and return the same choice of recovered pointers.
This does not assert that either arm belongs to an object: each arm uses
the same existence and offset checks, and can still be outside or null.
The search reaches through at most eight additions, and one conversion
splits at most 32 choices before falling back to the ordinary comparisons.

Subtraction now cancels a matching summand below nested additions:
`((base + 128*k) + 8) - base` becomes `128*k + 8`. This identity holds
modulo the bitvector width, without an assumption about overflow. The
search has a budget of 32 nodes; a sum it cannot cancel remains unchanged.
Unlike distributing every arithmetic operation over every choice globally,
splitting address choices only in `fromAddress` limits the extra terms to
the conversions that need them.

## Measurements

Side-by-side runs through `tests/bench.py`, with the same models and clang
for both binaries. These small times are rounded to hundredths of a second;
the whole-suite runs also share the machine with other tests.

| Test | Solve, main → branch | Peak MB, main → branch |
|---|---|---|
| `esbmc/github_9785_two_targets` | 0.28 → 0.01 s | 33 → 29 |
| `esbmc/github_9785_two_targets_fail` | 0.27 → 0.01 s | 33 → 29 |
| `vereda/int_address_bounded` | 0.01 → 0.00 s | 29 → 29 |
| `vereda/int_address_nested` | 0.07 → 0.01 s | 33 → 29 |
| `vereda/int_address_choice` | 0.14 → 0.02 s | 33 → 30 |
| `vereda/int_address_path_fail` | 0.02 → 0.02 s | 31 → 31 |

The other six non-skipped #9785 tests retain their verdict and solve in
0.00 s at this precision on both binaries. `bench.py` reports no regression
on these twelve tests. Dumps for the bounded and nested cases lose their
one pointer-selection `ite`; the choice case drops from eight `ite`s to
five. Object bases remain in the layout constraints, but no longer occur
in the recovered pointers' memory indices.

The new regressions cover bounded offsets and one-past pointers, nested
sums, choices between two objects and null, and a sibling branch with an
unbounded offset. In the latter, the integer address can reach a second
exposed object: its failing assertion must not disappear because of bounds
learned on the first branch. This also exercises wrapping integer offsets.

The full suite runs 6,991 tests: 2,809 pass, 4,182 are expected skips, and
none fail. Both binaries also ran the 859 tests selected by
`esbmc.*(9785|int|ptr|pointer)`: no expectation failures or verdict changes.
Of these, 819 produce statistics (the other 40 need unsupported builtins),
and the 382 reporting solve times in both sum to 4.03 s on main and 2.68 s
here. `bench.py --stats` finds no changes exceeding its regression or
improvement thresholds. The fixed twelve-test list above separately measures
the smaller gains relevant to this issue.
