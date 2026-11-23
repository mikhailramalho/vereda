# memcmp, bcmp and memchr in the walker

Recorded 2026-11-23.

The models of memcmp, bcmp and memchr in `models/string.c` are byte loops,
so a call cost one unrolled iteration per byte, and `--unwind` stopped a long
one with an unwinding assertion. ESBMC removed the same cost with intrinsics
(esbmc/esbmc#5342, #5345). The walker now reads the range whole and builds
the result as one term, back to front so that the first byte that decides
is outermost: memcmp's is the difference of the first differing bytes as
unsigned chars, memchr's a pointer to the first byte equal to
`(unsigned char)c`, else null. A symbolic length is handled when both
objects have a constant number of bytes left and the solver shows the length
stays within them; otherwise, and past `Memory::MaxBytes`, the model's loop
runs as before. Only the models are replaced: they carry no debug info, so a
program's own memcmp, compiled with `-g`, is still walked as written.

A program comparing and searching two 4096-byte buffers, with constant and
symbolic lengths, took 574 s (572 s of it walking the loops) and now takes
0.3 s, with the same verdict. Of the 38 imported tests that call these
functions, none changes time or memory beyond `tests/bench.py`'s margins;
one, `esbmc-cstd/github_9439_fail`, went from FAILED to SUCCESSFUL, because
its FAILED was only the unwinding assertion of a 64-byte memchr under
`--unwind 20`. It stays FUTURE: what it expects is a memory-safety failure,
which Vereda does not check yet.
