# Integer addresses inside one object (#224)

Recorded 2026-11-25.

`tests/esbmc/github_9785_int_base_stores.c` (imported from ESBMC #9785,
KNOWNBUG until this change) stores 16 `unsigned long` fields through
`(unsigned long)ports + i * sizeof(struct port) + 8k`, reads them back and
asserts that their sum is 120. On main at 2263b0d3 (LLVM 23, Bitwuzla 0.11)
the walk took 0.002 s and kept one check, yet the solve took 108.1 s and
9,312 MB, and 127.0 s and 9,308 MB with pointer checks on. The
`_fail` variant took 8.1 s and 456 MB. The dumped formula is 48 KB: 248
`store`, 130 `select`, 136 `bvadd` and 15 `ite`.

## Where the formula came from

The index is not the problem. LLVM turns `i < 0 || i >= 1` into
`i < 0 || i != 0` and puts 0 for `i` in the code after it, so the IR
already stores through `inttoptr (add (ptrtoint @ports), 8k)`, and #209's
ranges have nothing to do. A range of one value need not become a #163
equality for this test.

The cost is in `Memory::fromAddress`. For an address `a` it builds, per
exposed object, `inside = exists && a >= base && a <= base + size`, here
`base#1 <= base#1 + 8k && base#1 + 8k <= base#1 + 128`. The term builder
cannot fold that: `base#1 <= base#1 + 8k` holds only if the add does not
wrap, which the layout constraint `!uaddo(base#1, 128)` says but Terms does
not know. Each pointer is then `ite(inside, (ports, 8k), (outside,
base#1 + 8k))`. Its object is not a constant, so the store skips the
known-byte map and becomes 8 byte stores at 96-bit indices of the form
`concat(extract(ite ...), bvadd(extract(ite ...), j))`, and each load
becomes 8 selects at the same kind of index. The fifteen `ite`s are these
pointers; the store at offset 0 goes through `@ports` itself.

## Why the solver takes so long on it

Bitwuzla's statistics, from a small driver linking camada's
`libbitwuzla.a` (0.11) and parsing the dump, with a different build and
load from the runs above:

| Fields | Array lemmas | AIG ands | CNF clauses | SAT time | Peak |
|---|---|---|---|---|---|
| 8 | 64 | 372,593 | 502,026 | 32.8 s | 2.3 GB |
| 16 | 128 | 1,764,388 | 2,316,691 | 180.3 s | 11.2 GB |

The array part is cheap: one lemma per select. What remains is
bit-blasted. Each lemma equates or separates two indices that are 64-bit
adders over `base#1` under `ite`, `extract` and `concat`, which the
rewriter does not normalise, so CaDiCaL has to prove `base + a != base + b`
from the adders' circuits. Through the verifier (main, Bitwuzla 0.11, no
pointer checks), the solve time against the number of fields:

| Fields | 2 | 4 | 6 | 8 | 10 | 12 | 16 |
|---|---|---|---|---|---|---|---|
| Solve | 0.015 s | 0.14 s | 0.35 s | 20.1 s | 27.9 s | 58.0 s | 108.1 s |
| Peak | 40 MB | 57 MB | 79 MB | 2.0 GB | 2.7 GB | 5.2 GB | 9.3 GB |

The same 16-field dump takes Boolector 3.2.2 2.0 s and 23 MB (after
renaming `define-const` and rewriting `bvuaddo`, which it lacks). Z3,
through `--solver z3`, gave no answer in 300 s. ESBMC 8.7 resolves
the pointers in symex: with `--no-pointer-check --no-align-check
--no-bounds-check` it generates no VCC (0.4 s), and with its default checks
it keeps 96 of 192 and Bitwuzla 0.9 takes 2.0 s. So the formula reached
Bitwuzla only because Vereda's encoding left the outside case in. Bitwuzla
0.9 was not run on Vereda's formula.

## The change

Where an object exists its layout says `base + size` does not wrap, so
`base <= a <= base + size` holds exactly when `a - base <= size`, for any
`a`. If `a < base`, then `a - base` wraps to `a - base + 2^n`, and that
being at most `size` would put `a + 2^n` at most `base + size < 2^n`.
`fromAddress` builds `a - base` (the offset) first. `Terms::sub` folds
`(base + x) - base` to `x`. When the offset is a constant, the inside test
becomes `exists && x <= size`, which folds; for an object that exists on
every path and `x` within it, it folds to true. Where it does, the closed
ranges of existing objects are disjoint (each base is above the previous
end plus a gap), so no other object holds `a`, and `fromAddress` returns
the object at offset `x` without the chain. The pointer is then an
ordinary pointer to that object, not one made from an integer. When the
offset is not a constant, the test is main's two comparisons, unchanged.

## Measurements

Three binaries: main at 2263b0d3, the general form (`a - base <= size`
for every address, this PR's first version) and the change above.

Formulas. Every test `ctest -R 'esbmc.*(9785|int|ptr|pointer)'` selects
(861, of which 40 call an `__ESBMC_` or `__CPROVER_` builtin) was dumped
with `--dump-smt-only` under its `RUN` flags. An `--incremental-bmc` test
was dumped at `--unwind 3` in place of its bound flags, and a test whose
flags Vereda rejects (most FUTURE ones) without them, at `--unwind 3`. 745
gave a formula; the other 76 give none on main either (an unsupported
construct such as `pthread_create`, or an error). With the change, 741 of
the 745 are byte for byte main's. The other four are the ones where the
offset is a constant and folds: `github_9785_int_base_stores` and its
`_fail`, `intptr_struct_store_merged` (`base + 8` and `base + 40` into a
struct, 20,439 bytes to 2,674) and
`esbmc-k-induction/pointer_write_aggregate_arg_fail` (an address passed as
an integer and converted back, 6,176 bytes to 1,169). The general form
changed 65.

Times. The same 861 tests with main's binary and the change, 6 jobs
each: 328 pass and 533 are skipped as KNOWNBUG, FUTURE or `Needs:`, and no
verdict changes but the promoted test's, from out of memory to SUCCESSFUL.
Over the 383 tests that report a solve time with both, it sums to 30.0 s
on main and 21.1 s here, the difference being the `_fail` test.
`tests/bench.py`, side by side with main:

| Test | Solve, main → branch | Peak |
|---|---|---|
| `github_9785_int_base_stores` | out of memory → 0.00 s | 36 MB |
| `github_9785_int_base_stores_fail` | 7.18 → 0.00 s | 458 → 38 MB |
| `github_9785_two_targets` | 0.68 → 0.69 s | 48 MB |
| `github_9785_two_targets_fail` | 0.64 → 0.66 s | 50 MB |
| `ptr_int_mul_unknown_alias` | 2.54 → 2.56 s | 45 MB |
| `pointer_repr_branch_store` | 6.79 → 6.82 s | 56 → 57 MB |

On its own (12 GB limit) main takes 108.1 s and 9,312 MB on
`int_base_stores`, and 127.0 s and 9,308 MB with pointer checks on; with
the change every check folds during the walk either way.

The general form was faster overall (15.0 s summed over the same 383
tests), mostly through `pointer_repr_branch_store` (6.95 s to 1.01 s) and
a few tests gaining 0.3 to 1 s, but slower on three, side by side:
`github_9785_two_targets` 0.69 to 0.95 s, its `_fail` 0.66 to 1.67 s and
`ptr_int_mul_unknown_alias` 2.63 to 3.75 s. In `two_targets` the address
is `ite(c, base_a, base_b) + 4k`, which folds for neither object, and the
solver took longer on one comparison per object than on two. It was not
adopted, so that a formula changes only where the test folds.

## Not done

The fold needs `a - base` to be a constant. A symbolic offset
(`base + 4k` with `k` bounded only on the path) still leaves the outside
case to the solver, since Terms does not fold by a path's ranges. Nor does
it reach an address that chooses between objects (`two_targets`) or a sum
nested deeper than `(base + x) - base`, such as `(base + 128i) + 8`. The
general form's gain on `pointer_repr_branch_store` is left for a change
that can keep it without the slowdowns.
