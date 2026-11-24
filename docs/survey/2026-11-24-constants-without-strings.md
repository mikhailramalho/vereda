# Bit-vector constants without strings

Recorded 2026-11-24.

A perf profile of the walk on array_loop_summary_decrement_fail with
`--no-summarize-array-loops` showed about 5% of its samples in string
conversion (`APInt::toString`, `strtoull`, `__gmpz_set_str`) and in
`APInt::lshrSlowCase`. Every bit-vector constant was printed to a string
that the solver parsed back.

## Where the strings came from

`Terms::bv` is the only place Vereda builds a bit-vector constant; the
encoder and memory both go through it. It printed each new constant with
`APInt::toString` in base 2, padded it to the width and passed it to
camada's `mkBVFromBin`. camada checks the string and hands it to
`bitwuzla_mk_bv_value` in base 2. Bitwuzla checks it again
(`is_valid_bv_str`), parses it with GMP to see that it fits the sort
(`BitVector::fits_in_size`, even for a byte), and parses it a second time to
build the value (`std::stoull` up to 64 bits, `mpz_init_set_str` above). For
a constant wider than 64 bits, `toString` also shifts the whole APInt right
once per digit, which is the `lshrSlowCase`.

camada already has a constructor without a string: `mkBVFromDec(int64_t,
width)`. Since camada #69 its Bitwuzla backend masks the value to the width
and calls `bitwuzla_mk_bv_value_uint64` up to 64 bits; Z3 takes the integer
(`Context.bv_val`) and cvc5 too (`mkBitVector(width, uint64_t)`). `Terms::bv`
now passes constants up to 64 bits there, as their zero-extended value
converted to `int64_t`, which keeps the bits. Wider constants, pointers among
them (a 32-bit object and a 64-bit offset), still go as a string, the only
form Bitwuzla's API takes above 64 bits, but the string is built one bit at
a time instead of through `toString`.

On this benchmark the walk asks `Terms::bv` for 2.7 million constants and
builds 707,000 of them: 256 of 8 bits, 32,769 of 16, 100,001 of 24, 199,999
of 32, 100,000 of 62, 175,001 of 64 and 100,000 of 96.

## Time

Release builds with the installed LLVM 21, camada 0.24 and glibc's malloc
(no `MIMALLOC_SOURCE_DIR`), main at 1290acd4 with #166 cherry-picked into
both, so that the 100,001 solver calls it removes from this benchmark do not
hide the rest. The machine has 80 cores and ran under a load of about 25.

array_loop_summary_decrement_fail with `--no-summarize-array-loops`, base
and head run alternately, eleven runs each: the walk's median goes from
5.38 s to 5.14 s (the first three pairs: 6.01, 5.90, 5.62 s against 5.17,
4.83, 4.89 s). Counted by perf, which the load does not disturb, the walk
plus `--dump-smt-only` takes 18% fewer user instructions (14.97 to 12.27
billion) and 7% fewer user cycles (7.03 to 6.53 billion); a whole run, with
the solve and the counterexample, 6% fewer instructions (44.40 to 41.68
billion). The verdict and peak memory do not change.

Samples of the walk under `Terms::bv` fall from 39% to 31%. What is left
there is Bitwuzla making the value node and its C API's term table (about
43% of it), the two constant caches of `Terms` (`bvs`, the
`DenseMap<APInt, Ref>`, about 23%, and `values`, about 20%), and the 96-bit
constants' strings (69 samples, 3% of the walk).

`tests/bench.py --list` on 12_lms_new, 13_ludcmp_new, github_302,
github_1289 and the array_loop_summary tests, base against head: only
github_302 still walks for long, 55.64 s to 51.76 s (wall 79.99 s to
75.37 s); the others take under 0.2 s each with both binaries. The tests
bench.py lists as regressed end with the same error with both (FUTURE tests
that need a flag or builtin Vereda lacks).

## Not done

A camada change would only help the 96-bit constants: above 64 bits
Bitwuzla's C API takes nothing but a string, so camada cannot skip the
parse. A base-16 string would make it four times shorter, for at most the
3% of the walk those constants take. Not prototyped.
