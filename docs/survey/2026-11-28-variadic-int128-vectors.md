# i128 and vector variadic arguments

Recorded 2026-11-28.

An i128 or a vector argument to a variadic function is now placed where
the x86 ABIs put it, which is what clang's va_arg code reads. On x86-64 an
i128 takes two general registers when two are free (va_arg tests
gp_offset <= 32), else 16 bytes of the stack aligned to 16; an int after
it still takes a register left free. A vector of up to 16 bytes, such as
the <2 x float> clang makes of a struct of two floats or the two it makes
of a struct of four, takes an SSE register while one is free (clang makes
a struct byval when it needs two and one is left), else the stack aligned
to its type. On i386 a vector goes on the stack aligned to its type, 16 for
a 16-byte vector (clang's va_arg aligns with llvm.ptrmask). A vector is
stored as its elements' bits, the first lowest, as memory holds it.

Still unsupported: vectors of more than 16 bytes (on x86-64 clang passes
them byval, which works; on i386 a 32-byte vector is aligned to 32, more
than the 16 the walker's stack area has), vectors of pointers or long
doubles, i128 on i386 (clang has no __int128 there) and targets other
than x86.
