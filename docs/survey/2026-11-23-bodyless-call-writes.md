# What a call without a body writes

Recorded 2026-11-23.

A call to a function with no body and no model returned a nondet value and
left memory alone, so `int x = 0; sscanf(s, "%d", &x); assert(x != 5)` was
proved (issue #13). Such a call now gives nondet contents to the whole object
each pointer argument points to, unless the argument is a constant global or
LLVM's `inferattrs`, now the last pass before the models are linked, marks it
read-only (the format of printf, the source of sscanf, the buffer of fwrite).
Variadic pointer arguments get the same, except for the scanf family, whose
constant format says which argument gets how many bytes and which are written
for each result r, and the printf family, which writes none unless its
constant format has a `%n`. A format that is not a constant, or that names
arguments by position (`%1$d`), writes every argument's whole object, and
scanf's result is then any int. The format is read as glibc reads it: `%S`
and `%C` are `%ls` and `%lc`, `%llf` and `%qf` are `%Lf`, `%ms` writes a
pointer, and a conversion it does not know writes the whole object. `__VERIFIER_nondet_memory(p, n)` writes n nondet
bytes at p the same way.

A pointer whose object is not a constant term (loaded after a store at a
symbolic address, or returned by another bodyless call) is accepted when the
solver finds one object for it on the path; otherwise the verdict is UNKNOWN,
naming the call. Writing every object the pointer may reach under a guard
would be sound, but makes a pointer returned by a bodyless function, which
may point anywhere, wipe the whole memory, so it was not built.

stdin, stdout and stderr and the streams fopen, fdopen, tmpfile and freopen
return are now objects of a model (`models/stdio.c`), so that a stream
argument has one object; each opened stream is a new heap object, or null.
fmemopen and open_memstream stay bodyless: their streams write the program's
own buffer, which a fixed stream object would hide, so calls on them are
UNKNOWN. The pthread functions write nothing, as threads are not modelled.
ESBMC's and CBMC's builtins (`__ESBMC_*`, `__CPROVER_*`) get no special case,
as Vereda supports only the `__VERIFIER_*` ones: a call to one is a bodyless
call like any other, so tests that pass a pointer to one (`__ESBMC_same_object`,
`__CPROVER_r_ok`) may give UNKNOWN and are FUTURE with a `// Needs:` line. Writes
through pointers stored inside the objects, and to globals, stay unmodelled.

The first version made 05_pfscan-1.0_01 (--incremental-bmc, UNKNOWN on a
calloc of symbolic size) take 122 s where main took 17 s: its fprintf and
fwrite calls on stderr and stdout each asked the solver twice which object
the stream was, 13 queries of up to 42 s. A pointer loaded from a global
after a join did not fold to its value: the read stopped at the ite of the
two sides' arrays, and the zeroed FILE objects of the model had put 648
stores in front of it. A read at a constant index now goes through a join
when both sides hold the same value there, within 256 nodes in all (64
before), and the standard streams are declared, not defined, so their
contents are unknown and take no stores. No solver call is left there, and
the test takes 39 s. The rest is the feasibility query of calloc's size,
which main also runs and which is erratic: with main's binary, declaring
one to four unused 216-byte globals in the test's IR moves it between 4.7 s
and 63 s, as an unconstrained argv may point into any object.

On the imported ESBMC tests whose linked IR calls a bodyless function that
may write through a pointer (1,209 tests), 20 FUTURE tests now give the
expected verdict and became CORE: 19 had given SUCCESSFUL on a violated
property (14 of them scanf tests, the others strncat, timegm, getline and
wcscpy writing their buffers, and a pointer chosen by a branch), and
scanf_return_count had given FAILED. 16 CORE tests now give UNKNOWN and
became FUTURE: calls on fmemopen streams, getopt and inet_addr on argv,
gettimeofday, timegm and InterlockedIncrement through loaded pointers, a C++
stream, and calls to unknown function pointers. 9 FUTURE tests went from a
verdict to UNKNOWN and were re-marked. No test gives a wrong verdict it did
not give before.
