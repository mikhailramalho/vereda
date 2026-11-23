# Uninitialised arrays

Recorded 2026-11-23.

array-cav19/array_tiling_tcpy gave a wrong FALSE in 0.2 s (#9): it copies an
uninitialised variable-length array into another and asserts the copy
equals its source. The walker was not at fault: the untouched bytes of an
object already read back as one unknown value, which is why the same
program on malloc'd memory verified. SROA and InstCombine fold a load from
an alloca nothing has written to undef, so each read of a local array, fixed
or variable-length, became its own undef and the assertion folded to false
before the walk. nondetLocals had covered scalar locals only, on the
assumption that arrays stay in memory.

A local that holds an array with elements, and every dynamic alloca, now gets
a call to a bodyless `__vereda_uninitialised(ptr)` right after its alloca. It
is declared to write only through its argument and not to capture it, so
LLVM keeps the loads and alias analysis stays as precise as before; the
walker treats it as a no-op. Per-element nondet stores would do the same at
a cost that grows with the array.

A first version marked every local with a part that is not a scalar leaf,
which also kept bf16 and x86_fp80 locals and structs with a flexible array
member in memory instead of registers: six imported tests failed, four on
types the encoder cannot load and two on a pointer constant stored to memory
and read back as an integer (github_5393, zero_length_array_member_deref),
which does not give the integer back. That last one is a separate bug the
promotion had hidden; zero-length and flexible array members hold no bytes
to read, so they are not marked.

array_tiling_tcpy now reaches k = 20 in 120 s with no violation (S is
unbounded, so BMC cannot prove it; ESBMC times out too). tests/bench.py
against main shows no regression: times within noise (10_fir_new 0.86 s →
0.85 s, array_loop_summary_counter_fail 42.1 s → 40.8 s), peak memory 8 to
11 MB higher on every test, trivial ones included, which is the build
rather than the change (the base binary is another build of main).
