# Variadic calls

Recorded 2026-11-23.

Clang lowers va_arg itself on x86: on x86-64 it emits loads from the
va_list's reg_save_area at gp_offset or fp_offset, or from its
overflow_arg_area, and on i386 it bumps a char pointer. The walker
leaves that code alone and fills in what it reads. At a call to a defined
variadic function it places the IR arguments where the caller's ABI
puts them: on x86-64 the first six integers and pointers and the first
eight floats and doubles in a 176-byte register save area, the rest and
byval structs on the stack in 8-byte slots; on i386 all on the stack in
4-byte slots. Clang has already split a struct into such pieces, or made
it byval when the registers it needs are taken, so placing the IR
arguments one by one matches what the callee reads. va_start writes the
va_list: gp_offset and fp_offset past the named arguments' registers and
the two areas on x86-64, the first variadic stack slot on i386. va_copy
copies the 24 or 4 bytes, va_end does nothing. Pointers keep their
provenance, as the areas are ordinary memory objects.

Not adopted: lowering va_arg in IR before the walk. LLVM's ExpandVariadics
pass knows only the GPU and WebAssembly ABIs, and the callee's code
already reads the x86 layout, so a lowering would have to rewrite clang's
va_arg code too. Nor putting every variadic argument on the stack with
the registers marked as used (gp_offset 48, fp_offset 176), which is
simpler but wrong for an over-aligned struct that clang passes in two
registers: the callee would align the stack pointer first.

Arguments of another type (vectors, as for a struct of two floats on
x86-64; i128; long double) and other calling conventions are
unsupported. So are calls to vsprintf, vsnprintf, vasprintf and the
v*scanf, which have no model: as bodyless calls they would write nothing,
and were UNKNOWN before only because the variadic function calling them
was. An over-aligned struct read with va_arg on x86-64 needs
llvm.ptrmask, which the encoder does not have yet.

Of the 79 imported tests that use va_start or va_arg or define a variadic
function, 16 FUTURE tests now give the expected verdict and are CORE, and
2 more (va_arg on a va_list va_start never set) give FAILED, as ESBMC
expects, without ESBMC's message, so they stay FUTURE. In the ReachSafety
baseline, ldv-regression/fo_test (expected false) went from UNKNOWN to
FALSE.
