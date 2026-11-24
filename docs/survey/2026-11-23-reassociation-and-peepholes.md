# Reassociation, canonical order and ESBMC's peepholes in Terms

Recorded 2026-11-23.

Issue #69 asked to port the reassociation of add, mul and bitwise chains
from ESBMC's simplifier rewrite (esbmc/esbmc#4282) into `encode/terms`,
with the bit-vector peepholes of that pull request, and to check our
folding rules against the soundness bugs it fixed. Of all of it one
peephole is kept: an unsigned or signed comparison against the least or
greatest value of its type folds (`x < 0` is false, `0 <= x` true, and so
on for UINT_MAX, INT_MIN and INT_MAX).

## Soundness

The bugs #4282 fixed, against what Vereda does:

| #4282 fix | here |
|---|---|
| `(x * c) == 0` to `x == 0` only for odd c | no such rule |
| `p - p` gave a pointer-typed zero | terms have widths, not types; `sub(a, a)` is a zero of a's width |
| constant folds across operands of different widths (`(x_u8 + c_u16) == c2`, mixed pointer offsets, `from_integer` not truncating) | every operation takes equal widths, constants are APInts of that width, and `bv(int64_t, width)` refuses a value that does not fit (#59); getelementptr offsets are summed by LLVM at pointer width |
| bitwise folds through 64-bit integers lost bits past 64 | APInt at any width |
| combined constant shifts wrapping the shift count | no shift combining (see below for the one tried) |
| popcount, bswap of negative constants | not folded here; any intrinsic the encoder does not know is unsupported |
| bitcast chains collapsed across widths | casts are extract, zext and sext; extract of an extension folds only inside the source's bits |
| `byte_extract(byte_update)` at an out-of-bounds offset | `select` skips stores only at other constant indices of the same index sort, which is exact |
| `constant_array` turned into `array_of`, so out-of-bounds reads gave the fill value | `constArray` is an SMT constant array; reading it gives its value at every index, which is the array's own meaning, not a fold |
| `x % x` to 0 hid the division-by-zero check | no such rule; division and remainder by zero are left to the solver |
| `if(c, x, x)` to x, `0 << n` to 0 hiding overflow or shift checks | those checks are clang's sanitizer instrumentation, IR instructions of their own, so folding a value never removes one |
| bitfield and union rewrites, vector-typed scalar shortcuts | no bitfields, unions or vectors at this level (the scalarizer runs first) |

None applies. To check the rules themselves rather than read them, a
differential fuzzer (not in the tree) built random terms of 4, 8 and 72
bits twice, once through `Terms` and once with the solver's own
constructors, and asked Bitwuzla whether the two can differ: every
operation, ite with constant branches (so `lift` fires), extract windows of
concats and extensions, reads after stores into symbolic and constant
arrays, and the chain shapes `x + (y - x)`, `(x + y) - x`. 54,000 terms
over main's rules before #109, and 60,000 over #109's guard folds with
the peepholes below (and shapes built to hit each) gave no difference; breaking one rule (`(x + y) - x` to x)
was caught within 33 terms. The 72-bit runs mostly time out in the solver
on division, so they cover far fewer terms. The ESBMC regression tests
#4282 added are in the suite already (tests/esbmc/simplifier-*,
ptr_sub_unsigned_reassoc, if_cond_overflow_check, ub_shift_zero_lhs,
abs_int_min_overflow) and pass; simplifier-modulus-fail stays FUTURE since
InstCombine folds `x % x` to 0 before the walk.

## Reassociation (earlier, branch perf/reassociate, e5206e9d)

Folding the constants of add chains as they are built (the constant kept on
the right at the top of a chain, `x - k` as `x + (-k)`, `x + (y - x)` and
`x - (x - y)` cancelling, `x + k == c` as `x == c - k`) changed no verdict
and no test past bench.py's margins over the suite, with formulas within
0.3%. Doing the same for mul, and, or and xor changed nothing more. LLVM's
Reassociate in the pass list made github_9785_int_base_stores_fail run out
of time and left 498 more dbg.value records as poison. None of it was
merged.

## Canonical order

Measured before building it: at the end of each walk, every node was given
a key with add/sub, mul, and, or and xor chains flattened through nested
nodes of the same kind, their constants folded, `x + (-x)`, `x ^ x`, `x & x`
cancelled, and the operands sorted; nodes with equal keys are terms a
canonical order would merge. Over the 3,702 walks of the suite that reach
the end (the rest stop at unsupported IR, a timeout of 60 s, or before
walking):

| | nodes |
|---|---|
| all nodes | 1,972,256 |
| in an add, mul or bitwise chain | 64,283 |
| equal once commutative operands are sorted | 130, in 15 walks |
| equal once chains are flattened, folded and sorted | 36,668, in 55 walks |

35,826 of the 36,668 are in the six esbmc-field-sensitivity/global_callee_w*
tests (5,971 each), whose chains are `+ 1` steps through a struct: the
constant folding e5206e9d did, which those tests do not need (each runs in
0.2 s). Without them, 842 nodes in 49 walks, 0.04% of all nodes. Two
orderings of one sum are rare because clang's InstCombine has already put
each function's arithmetic in its own canonical order; the walker only adds
chains across iterations and calls, and those are built in program order
every time. Not built.

## Peepholes

The #4282 peepholes that apply to bit-vectors, added together with a
counter each, over the suite (3,702 walks):

| rule | fires | walks |
|---|---|---|
| `0 << x`, `0 >> x` to 0 | 0 | 0 |
| `(x << c1) << c2` to `x << (c1 + c2)`, count saturated at the width | 39 | 1 |
| `x ^ ~0` to `~x` | 82 | 14 |
| `~x ^ ~y`, `x ^ ~x`, `(x ^ y) ^ y`, `(x ^ c1) ^ c2` | 0 | 0 |
| `x & ~x`, `x | ~x`, `(x & y) & y`, `(x | y) | y` | 1 | 1 |
| `~x == ~y`, `x ^ c == k`, `(x * odd) == 0` | 4 | 3 |
| `d + c == d + e`, `d - c == d - e` | 12 | 9 |
| `x + c == k` to `x == k - c`, `x - c == k` | 137 | 26 |
| comparison against the least or greatest value | 687 | 32 |
| `ite(c, ite(c, a, b), x)` and the three other nestings | 566 | 44 |
| `x - (x + y)`, `x - (x - y)`, `-1 - x` | 27 | 7 |

Side by side with main's binary (tests/bench.py) on the 112 tests where any
of them fired, nothing moved past bench.py's margins, and only
esbmc/promoted_unsigned_nonneg moved past noise: 0.57 s to 0.13 s, peak
143 MB to 77 MB. That test is the comparison rule's case: `ref >= count`
with count a global that is still zero, which clang cannot fold since it
does not know count is never written; the walker loads the zero, and with
the rule the guard is true, so each loop's bound is the constant 3 and not
a choice between 3 and 30,000.

On the Arrays and Loops tasks of the ReachSafety baseline
(2026-11-23-reachsafety, 1,198 tasks, 30 s each, both binaries side by
side, with Vereda's incremental BMC as the baseline runs it): with every
peephole above against main (12db36d3), both gave 295 SUCCESSFUL, 215
FAILED, 7 UNKNOWN and 681 timeouts; two tasks swapped a verdict for a
timeout at the 30 s line, one each way, and agree at 60 s. The 516 tasks
both finished took 853.6 s and 843.8 s. Most of the rules fire there only
as `x + c == k` on loop counters and nested ites, and the comparison rule
not at all. The largest moves, measured again at 60 s, were
nla-digbench-scaling/egcd2-ll_valuebound2 and _valuebound5 (16.0 s to
9.6 s, 37.9 s to 21.9 s), which come from one rule alone, `x - (x + y)`
to `0 - y` (it fires 10 times there): with only that rule added, the two
take 10.2 s and 22.3 s, but egcd2-ll_unwindbound5 goes from 39.3 s to
52.5 s and _valuebound1 from 3.3 s to 3.8 s. The solver reacts to an
equivalent formula of another shape, both ways; dropped.

Kept: the comparison rule alone, in `ult`, `ule`, `slt` and `sle`. Side
by side on the 32 tests where it fires, promoted_unsigned_nonneg walks in
0.00 s instead of 0.40 s (0.14 s instead of 0.61 s in all, 77 MB instead of
143), and the rest are unchanged. tests/vereda/fold_extreme_compare.c pins
the fold (every check is false before solving, which main's binary fails),
and fold_extreme_compare_fail.c that one past either bound still fails.
