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
being at most `size` would put `a + 2^n` at most `base + size < 2^n`. So
`fromAddress` now tests `exists && a - base <= size`, with `a - base` the
offset it already built. `Terms::sub` folds `(base + x) - base` to `x`. For
a constant `x` within an object that exists on every path, the test folds
to true. Where it does, the closed ranges of existing objects are disjoint
(each base is above the previous end plus a gap), so no other object holds
`a`, and `fromAddress` returns the object at offset `x` without the chain.
The pointer is then an ordinary pointer to that object, not one made from
an integer.

An earlier version used the new test only when `a` was the base or an add
with the base as an operand, through a new `Terms::sum`. The general form
is shorter, and the measurements below favour it overall.

## Measurements

The 861 tests `ctest -R 'esbmc.*(9785|int|ptr|pointer)'` selects, with
main's binary and this branch's run on the same machine, 6 jobs each. 328
pass and 533 are skipped as KNOWNBUG, FUTURE or `Needs:`; no verdict
changes except the promoted test, which goes from out of memory to
SUCCESSFUL. Over the 383 tests that report a solve time with both, it sums
to 30.0 s on main and 15.0 s here. `tests/bench.py`, side by side, finds no
regression; the tests that changed:

| Test | Solve, main → branch | Peak |
|---|---|---|
| `github_9785_int_base_stores` | out of memory (9.3 GB) → 0.000 s | 36 MB |
| `github_9785_int_base_stores_fail` | 7.83 → 0.00 s | 456 → 37 MB |
| `pointer_repr_branch_store` | 6.95 → 1.01 s | 56 → 54 MB |
| `pointer_repr_symbolic_byte_keep` | 1.78 → 0.80 s | 54 → 52 MB |
| `vasprintf_unbounded_s_no_overflow` | 2.59 → 2.01 s | 82 → 51 MB |
| `intptr_struct_store_pointer` | 0.74 → 0.48 s | 54 → 48 MB |
| `intptr_struct_store_nested_union` | 0.58 → 0.20 s | 50 → 46 MB |
| `vereda/mem_inttoptr` | 2.34 → 1.81 s | 78 → 62 MB |
| `github_9785_two_targets` | 0.69 → 0.95 s | 48 → 45 MB |
| `github_9785_two_targets_fail` | 0.66 → 1.67 s | 50 → 46 MB |
| `ptr_int_mul_unknown_alias` | 2.63 → 3.75 s | 46 → 50 MB |

The last three got slower, under bench.py's margins. In `two_targets` the
address is `ite(c, base_a, base_b) + 4k`, which folds for neither object;
the solver gets one comparison per object instead of two, and takes
longer. With pointer checks on, the promoted test also folds every check
during the walk (0.000 s against main's 127.0 s).

## Not done

The fold needs `a - base` to be a constant. A symbolic offset
(`base + 4k` with `k` bounded only on the path) still leaves the outside
case to the solver, since Terms does not fold by a path's ranges. Nor does
it reach an address that chooses between objects (`two_targets`) or a sum
nested deeper than `(base + x) - base`, such as `(base + 128i) + 8`.
