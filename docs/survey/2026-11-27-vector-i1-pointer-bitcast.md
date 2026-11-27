# Vectors of i1 and pointers, and vector bitcasts

Recorded 2026-11-27.

Follow-up to `2026-11-22-vectors.md` (#304). The scalarizer leaves a
vector only where it crosses a call, and there a vector of i1 or of
pointers, or a bitcast between a vector and a scalar, gave UNKNOWN. A
vector of either is now the same one bitvector as other vectors: an i1
element is one bit (bit i is element i, as `bitcast <8 x i1> to i8` and
x86's mask instructions have it), a pointer element is the pointer term's
bits, object and offset, so a pointer taken out of the vector is the one
put in. Pointer vectors raise no provenance question here: the scalarizer
splits their loads and stores into element ones, which memory tracks as
any pointer store. A bitcast to or from a vector keeps the bits, lane 0
lowest, as storing and loading would on a little-endian target. Vector
global initialisers of i1 still stay unsupported: memory packs their bits,
which the element pointers do not read.
