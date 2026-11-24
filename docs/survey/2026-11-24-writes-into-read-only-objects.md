# Writes into string literals and const objects

Recorded 2026-11-24. Issue #133.

## What ESBMC reports

ESBMC 8.7 checks a write through a pointer in
`dereferencet::read_only_write_check` (`src/pointer-analysis/dereference.cpp`):
the object is read-only when it is a string literal (`constant_string2t`) or a
symbol with static lifetime whose type is const (`is_const_static`), which
covers const globals and `static const` locals but not automatic const
locals. The claim is an assertion like any other, worded
`dereference failure: write access to string constant` or
`dereference failure: write access to const object `name'`, and it is part of
the pointer check, so `--no-pointer-check` turns it off. ESBMC's SV-COMP
wrapper passes that flag for unreach-call and no-overflow. It reports a write
made by `__VERIFIER_nondet_memory`, memcpy, memmove, memset and scanf inside
its own library bodies, and nothing for a call without a body given a pointer
to a const object (`f((void *)&c); f("abc");` is SUCCESSFUL).

## Which objects are read-only

A global LLVM marks constant. Clang marks exactly the objects with static
storage of a const-qualified type (globals, `static const` locals, const
compound literals at file scope), string literals and `__func__`, and its own
copies of local initialisers (`__const.main.buf`), which the program only
copies from. No pass in Vereda's list can mark another global constant:
globalopt, IPSCCP and the attributor are not run, and InstCombine only
replaces a local by such a copy when nothing writes the local (and the
`__vereda_uninitialised` call on each local keeps it from doing even that).
An `.ll` input from an optimising pipeline may hold a global globalopt made
constant, but globalopt does so only when nothing writes it. So the check
needs neither debug info nor clang's `.str` names to decide; it uses them
only for the wording: an `unnamed_addr` global (a literal, `__func__`) is a
string constant, any other is named by its debug-info variable, else by its
IR name.

## What had to change besides the check

InstCombine deletes a store, memset, memcpy or memmove into constant memory
as undefined behaviour, so `*(int *)&c = 5` never reached the walker. The
`ir` stage now makes the constant globals such a write may reach (by
`getUnderlyingObjects`) writable while InstCombine runs and constant again
after. A library call InstCombine turns into such a copy (strcpy or strcat
from a constant string into a literal) is still deleted.

A call without a body may write the objects it gets, which is no write the
program surely makes, so it is never checked, and it no longer writes a
constant object at all: a pointer that chose between a local array and a
const object used to give the const object nondet contents.

The writes checked are those that happen: stores, atomics, a compare-exchange
where it stores, memcpy, memmove and memset of at least one byte,
`__VERIFIER_nondet_memory` of at least one byte, the models'
`__vereda_havoc` (getline, readv) and each scanf conversion where it assigns.
A pointer made from an integer is not counted: it points into an object only
because the solver may place the object at that address. Under `--property`,
where the check does not count, a write into a read-only object ends its
path, as any other failure there does.

## Imported tests

Seventeen FUTURE tests now give ESBMC's verdict and were promoted:
`const_object_write_{memcpy,memmove,nondet_memory,sscanf}_fail`,
`string_literal_write_nondet_memory_fail`, `github_10812_scanf_literal_fail`,
`memset-const`, `memset-const-2`, `memset-const-no-simpl`,
`memset_string_literal`, the six `github_1548*` tests and
`25_git-remote-gitkrb5`, which no longer reaches an unsupported intrinsic
once a call without a body leaves a const object alone. Two,
`{const_object,string_literal}_write_memcpy_symbolic_fail`, went from a wrong
SUCCESSFUL to UNKNOWN: the memcpy InstCombine used to delete has a size of
several values. 81 imported tests whose test.desc passes `--no-pointer-check`
got it on their RUN line, as the import now translates it; one of them,
`31_cdaudio`, failed without it: a literal's address is stored through a
pointer a call without a body returned, which may land in the IRP the
program later copies into.

## Cost

On 1223 imported tests (those of esbmc-unix, esbmc-unix2, esbmc-nonz3 and
esbmc-linux, and those that mention const data, scanf, sprintf or a memory
function), run with the check and with `--no-pointer-check`, 6 at a time,
60 s each: walk 9.9 s against 9.5 s, solve 43.5 s against 41.2 s, and no
verdict changed but the 16 tests that write into a read-only object. Most
writes fold the check away while walking, as their pointer's object is
known; the cost is in writes through a pointer loaded from memory when the
program exposes a literal, such as `27_exStbFb` (5 checks, 0.48 s of
solving that `--no-pointer-check` does not spend).
