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

Now object 0, at base 0, is null at offset 0 and the memory outside the
program at any other offset, which is the address: what main already gave
an address no object held. Every pointer from the list above is a fresh
address converted by `fromAddress`, and unwritten bytes hold integers
(provenance arrays start as constant arrays of the integer provenance), so
a pointer read from them is converted the same way. `fromAddress` maps an
address to an object only if that object is exposed and the path allocated
it, otherwise to object 0.

The walker decides exposure per allocation site with LLVM's capture
tracking: an alloca, global or address-taken function is exposed when its
address is captured in any way but a test against null (stored, passed to a
call that does not promise `captures(none)`, converted with ptrtoint,
returned, compared), or it appears in a global's initialiser, or it is a
global the program only declares (#152: the code that defines it knows its
address). Constant getelementptrs and casts of a global are followed as
instructions would be, since LLVM counts any constant user as a capture.
Heap objects and the va_list areas are always exposed. This is sound
because only an exposed object's pointer is ever stored in memory or turned
into an integer, and a call without a body can only return a pointer it was
given, stored where it was given one, or one of its own: the same rule #111
follows for what such a call writes. A pointer from outside therefore
points to null, to the outside memory or to an exposed object, and two of
them may still alias (both are offsets in object 0), which ESBMC's
per-dereference failed symbols do not allow.

Comparisons. A pointer whose address the program compares is exposed
because the imported ESBMC tests rely on an integer or nondet address
being able to equal any object's address: a constant address against a
string literal or a function (github_1539, github_1620,
function_address_signal_sentinels_fail), a pointer read from nondet bytes
or left uninitialised assumed equal to a global and then read through
(github_2153_*, github_8517_assume_pins_pointer*). An object whose address
is only tested against null cannot be the target of such an assume, so null
tests do not expose. Pointers compare by address when either is outside
(object 0 at a nonzero offset), since one converted before an object was
allocated may hold its address (github_7855-reread-later-object;
mem_ptr_eq_address, KNOWNBUG since #75, now passes). An object exposed at conversion time is never at an address
converted to object 0, so this does not let an outside pointer equal an
object it cannot point into.

A first version put the outside memory in a new object 1 at base 0, with
`fromAddress` choosing null for address 0 and object 1 otherwise. That
extra choice in every converted address made Bitwuzla take 63 s and 4.5 GB
on github_9785_int_base_stores_fail (main: 12 s, 0.5 GB); object 0 gives
back main's formula there. It also put the null/outside choice in every
pointer comparison and stored pointer. Comparing by address whenever one
side had object 0, null included, made every null test reason about bases
and doubled the solving time of the LDV task dell-led (24 s on main, 60 s).

Objects a path did not allocate. `fromAddress` also checks that the object
exists on the path: elsewhere its layout is unconstrained, so its base may
be 0, and a null read back from zeroed memory could convert into it as a
non-null pointer (`mem_null_absent_object`, FAILED on main). LLVM turns the
LDV final-state checks `(unsigned long)usb_urb == 0` into `usb_urb ==
NULL`, so main fails them on paths where a module's init fails and some
`ldv_zalloc` failed; with this check alone on main (and nothing else of
#137), 13 of main's 19 right FAILED on the LDV tasks expected false stop
being found that way and time out or stop at an unsupported construct
instead, and 6 of its 50 wrong FAILED go away.

What follows from that:

- The memory model knows which objects such a pointer may reach, so an
  access through a pointer whose object is not constant flushes the known
  bytes of only those objects (every exposed one, and object 0, for a
  pointer read from memory), and `havoc` writes each of them where the
  pointer points into it. A call without a body no longer gives UNKNOWN for
  "an object chosen at run time", and the walker no longer asks the solver
  whether the pointer has one object (that query alone ran
  esbmc/10_bicycle_01 past the time limit).
- A call without a body that may write memory other than its arguments also
  writes the outside memory as a whole, as #152 has it write errno and
  declared globals.
- The asm refusal of #127 (an atomic through a pointer to an unknown object)
  is gone: such a pointer can no longer reach state the program kept to
  itself, so the atomic is as sound as a C store.

## LDV

146 LDV unreach-call tasks, 60 s and 6 GB each, 12 at a time: the 95 that
main decided in `2026-11-24-inline-asm-after-variadics.md`'s run, the 11
wrong FAILED of #126, and 40 others at random. Main is cfeccd0b.

| | main | this change |
|---|---|---|
| FAILED, expected true | 50 | 0 |
| SUCCESSFUL, expected true | 25 | 25 |
| FAILED, expected false | 19 | 7 |
| timeout, or killed shortly before 60 s | 25 | 109 |
| UNKNOWN | 27 | 5 |

No wrong verdict is left, and the 25 right SUCCESSFUL stay (dell-led
takes 40 to 47 s against main's 24 to 30 s). The 50 wrong FAILED now time
out, `intel_vr_nor` and `altera_jtaguart` among them; 44 of them still fail
on main with only the allocation check, so they come from pointers from
outside reaching objects the program kept. 12 of main's 19 right FAILED on
tasks expected false are lost: main found them only through the null that
converted into an object its path never allocated (main with only the
allocation check loses 13 of them the same way). 23 of main's 27 UNKNOWN
(an asm through an unknown object, an unknown function pointer target, a
bodyless call on such a pointer) now run to the time limit; 5 tasks stop at
an intrinsic, a memcpy of symbolic size or an asm that may write memory.

## pfscan

`tests/esbmc-unix/05_pfscan-1.0_01.c --incremental-bmc` (KNOWNBUG), two runs
each: main stops after 7.7 to 8.2 s (510 MB) with UNKNOWN at an fprintf
whose stream "may write an object chosen at run time"; with this change it
gets past every bodyless call on argv's and the streams' pointers and stops
after 0.4 s (100 MB) at pthread_create, which #143 refuses.

## Tests

`mem_words_unknown_fail`, where a pointer made from a nondet integer wrote
into an array of int elements, now gives SUCCESSFUL: that array is never
exposed, and no array of elements can be (it is reached only through its
own getelementptrs). It is now `mem_words_unexposed`.
`esbmc/ptr_int_mul_unknown_wild` (FUTURE) goes from FAILED to SUCCESSFUL
for the same reason: its store through an address below 64 cannot reach
`c`, whose address the program never gives out; ESBMC's expected FAILED is
the invalid-pointer dereference, which Vereda does not check. 20 imported
FUTURE tests now pass and are CORE (among them fputs_unlocked, github_9391,
31_cdaudio, github_4978, pointer_repr_branch_store), and others went from
UNKNOWN to FAILED where the bodyless function they call (tolower_l,
ferror_unlocked, __fpending, timer_create, mktime) has no model.
`esbmc-cstd/scanf-false-3`, CORE but timing out on main, now stops with
UNKNOWN and is FUTURE. `esbmc/10_bicycle_01` gives UNKNOWN at
pthread_create in 0.5 s, as main does at gettimeofday.
