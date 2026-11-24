# What a call without a body writes

Recorded 2026-11-23.

A call to a function with no body and no model returned a nondet value and
left memory alone, so `int x = 0; sscanf(s, "%d", &x); assert(x != 5)` was
proved (issue #13). Such a call now gives nondet contents to the whole object
each pointer argument points to, unless the argument is a constant global or
LLVM's `inferattrs`, now the last pass before the models are linked, marks it
read-only (the format of printf, the source of sscanf, the buffer of fwrite).
Variadic arguments stay untouched except for the scanf family, whose
constant format says which argument gets how many bytes and which are
written for each result r; a format that is not a constant writes every
argument's whole object. `__VERIFIER_nondet_memory(p, n)` writes n nondet
bytes at p the same way.

A pointer whose object is not a constant term (loaded after a store at a
symbolic address, or returned by another bodyless call) is accepted when the
solver finds one object for it on the path; otherwise the verdict is UNKNOWN,
naming the call. Writing every object the pointer may reach under a guard
would be sound, but makes a pointer returned by a bodyless function, which
may point anywhere, wipe the whole memory, so it was not built.

stdin, stdout and stderr and the streams fopen, fdopen, tmpfile and freopen
return are now objects of a model (`models/stdio.c`), so that a stream
argument has one object; every opened stream is the same object, or null.
fmemopen and open_memstream stay bodyless: their streams write the program's
own buffer, which a fixed stream object would hide, so calls on them are
UNKNOWN. The pthread functions and `__CPROVER_uninterpreted_*` write nothing:
threads are not modelled, and an uninterpreted function is pure. Writes
through pointers stored inside the objects, and to globals, stay unmodelled,
as does printf's `%n`.

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
