# Variadic arguments aligned past 16

Recorded 2026-11-28.

The walker's stack area for variadic arguments was always aligned to
16, the stack's alignment at a call. An argument aligned to more needs
the area aligned to as much, since clang's va_arg code rounds the
pointer up to the argument's alignment (llvm.ptrmask with -32): a
32-byte vector on i386, which the walker refused, and a struct aligned
to 32 passed byval on x86-64, which it read from the wrong offset, so
the run gave a wrong FAILED. The area is now aligned to the largest
alignment among the arguments in it, and i386 takes a vector of
integers or floats of any size.

Native runs agree for the vector with -mavx and with gcc: a 32-byte
vector between two ints comes through intact. Clang without AVX puts
it at offset 16 on the caller's side while its va_arg code rounds to
32, so that program reads the wrong bytes; the walker follows the
va_arg code, which gives the value passed.

Still unsupported: vectors of long doubles on i386 (x86-64 passes them
byval), vectors of pointers (C cannot write them; they come only from
IR) and targets other than x86.
