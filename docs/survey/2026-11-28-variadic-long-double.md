# Long double variadic arguments

Recorded 2026-11-28.

A long double argument to a variadic function is now placed where the x86
ABIs put it. On x86-64 it is of class X87, so it always goes on the stack,
in 16 bytes aligned to 16, even while general and SSE registers are free;
clang's va_arg code aligns overflow_arg_area with llvm.ptrmask before it
reads one. On i386 it takes 12 bytes of the stack, aligned to 4. A stack
argument is aligned to its type's ABI alignment, at least the slot size,
which changes nothing for the types allowed before. The printf model
already read a long double for %Lf and the other floating conversions
with L; such a call no longer stops at the variadic call.

Still unsupported (issue #305): vector arguments (clang passes a struct of
two floats on x86-64 as one <2 x float> in an SSE register), i128 (two
general registers when two are free, else 16 bytes of the stack aligned to
16) and other calling conventions.
