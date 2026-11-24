# Pointers from outside the program

Recorded 2026-11-24.

Issue #137. Before this change a pointer Vereda did not build from one of its
own objects could point to any object, so a store through it could write any
of them. Such pointers came from:

- `Encoder::fresh` on a pointer type: main's pointer parameters (`argv`), the
  result of a call without a body (`ioremap`), `__VERIFIER_nondet_pointer`,
  an uninitialised pointer local (`__VERIFIER_nondet_uninitialised.ptr`), a
  pointer output of an asm, `stacksave`, and `undef`. Each was a free
  bitvector, its object id included.
- `Memory::fromAddress` (inttoptr, and a load of bytes that hold an integer):
  an address converted to the object whose range holds it, among every
  object. Bases are symbolic, so the solver could place any object at a
  constant MMIO address.
- Bytes nothing wrote: the initial provenance array, an `own()` object's
  initial one and the arrays `havoc` gives a whole object were free arrays,
  so a pointer read from uninitialised or havocked memory had any object.

Now object 1 is the memory outside the program: base 0, as large as the
address space, outside the layout, so its offset is the address. Every
pointer from the list above is a fresh address converted by `fromAddress`,
and unwritten bytes hold integers (provenance arrays start as constant
arrays of the integer provenance), so a pointer read from them is converted
the same way. `fromAddress` maps an address to an object only if that object
is exposed; otherwise to null (address 0) or to object 1.

The walker decides exposure per allocation site with LLVM's capture
tracking: an alloca, global or address-taken function is exposed when its
provenance is captured (stored, passed to a call that does not promise
`captures(none)`, converted with ptrtoint, returned) or it appears in a
global's initialiser. Constant getelementptrs and casts of a global are
followed as instructions would be, since LLVM counts any constant user as a
capture. Heap objects and the va_list areas are always exposed. This is
sound because only an exposed object's pointer is ever stored in memory or
turned into an integer, and a call without a body can only return a pointer
it was given, stored where it was given one, or one of its own: the same
rule #111 already follows for what such a call writes. A pointer from
outside therefore points to null, to the outside memory or to an exposed
object, and two of them may still alias (both are offsets in object 1),
which ESBMC's per-dereference failed symbols do not allow.

What follows from that:

- The memory model now knows which objects such a pointer may reach, so an
  access through a pointer whose object is not constant flushes the known
  bytes of only those objects (every exposed one, for a pointer read from
  memory), and `havoc` writes each of them where the pointer points into it.
  A call without a body no longer gives UNKNOWN for "an object chosen at run
  time".
- The asm refusal of #127 (an atomic through a pointer to an unknown object)
  is gone: such a pointer can no longer reach state the program kept to
  itself, so the atomic is as sound as a C store.
- A pointer outside the program is stored as an integer and converted again
  at the load (#75), like one with object 0 and an offset.

## LDV

146 LDV unreach-call tasks, 60 s and 6 GB each, 12 at a time: the 95 that
main decided in `2026-11-24-inline-asm-after-variadics.md`'s run, the 11
wrong FAILED of #126, and 40 others at random. Main is 6c600af6 (#111
included).

| | main | this change |
|---|---|---|
| FAILED, expected true | 50 | 5 |
| SUCCESSFUL, expected true | 25 | 25 |
| FAILED, expected false | 19 | 19 |
| timeout, or killed between 38 and 60 s | 26 | 94 |
| UNKNOWN | 26 | 3 |

No task lost a right verdict. The other 45 wrong FAILED now time out,
`intel_vr_nor` and `altera_jtaguart` among them. (A first run, before
`candidates` kept the constant sides of a choice whose other side is read
from memory, gave the same verdicts, with 5 of the 45 killed shortly before
60 s.) 23 of the 26 UNKNOWN (an asm
through an unknown object, an unknown function pointer target, sprintf) now
run to the time limit; the other 3 stop at an intrinsic or an asm that may
write memory, as before. The 5 still FAILED (esd_usb2, cdc_mbim, wlcore_spi,
com90xx, rtl8192se) fail in `ldv_check_final_state` after the module's init
fails; they fail the same with bodyless calls writing nothing, so they come
neither from pointers from outside nor from #111.

## pfscan

`tests/esbmc-unix/05_pfscan-1.0_01.c --incremental-bmc` (KNOWNBUG), two runs
each: main stops after 2.9 to 3.4 s (290 MB) with UNKNOWN at an fprintf
whose stream "may write an object chosen at run time"; with this change it
gets past every bodyless call on argv's and the streams' pointers and stops
after 6.1 to 6.4 s (320 MB) at "calloc with a symbolic size", the place the
first version of #111 reached (in 39 s, by
`2026-11-23-bodyless-call-writes.md`).

## Tests

On the imported tests marked with a verdict that #111 touched or that gave
UNKNOWN (120), 14 FUTURE tests now pass and became CORE (fputs_unlocked,
freading_fail, github_9391, 26_exStbKey, ...), and 9 went from UNKNOWN to
FAILED, where a bodyless function they call (tolower_l, ferror_unlocked,
__fpending, timer_create, mktime) has no model and returns a nondet value.
`mem_words_unknown_fail`, where a pointer made from a nondet integer wrote
into an array of int elements, now gives SUCCESSFUL: that array is never
exposed, and no array of elements can be (it is reached only through its
own getelementptrs). It is now `mem_words_unexposed`.
