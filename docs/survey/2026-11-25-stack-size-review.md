# Stack-size review follow-up (#243)

The importer now preserves the four adaptations made for #222. The two
`ptr_rel_huge_object_{alloca,vla}` tests disable the size report because
they test pointer relations under ESBMC's assumed bound on symbolic stack
sizes. The two `vla_{above_ptrdiff_max,no_size_check_fail}` tests translate
ESBMC's size-error wording to Vereda's alloca wording. These changes apply
only to those tests; expectations for other allocation mechanisms keep
their original meaning. Re-importing all four from the local ESBMC tree
produces files byte for byte identical to the committed copies.

When the element size exceeds PTRDIFF_MAX, the pre-multiplication limit
is zero. Only a zero count survives the guard, so the allocated size is
now explicitly the constant zero, including for symbolic counts. This
avoids truncating an element size at least as large as the pointer's
address space and avoids leaving a symbolic multiplication in memory's
size. Zero-size elements also use the explicit zero. A new IR regression
uses a symbolic count of 4 GiB elements with 32-bit pointers and confirms
that disabling the report still prunes every nonzero count.

Validation: all 536 tests selected by
`^vereda/|esbmc/(ptr_rel_huge_object|vla_|alloca_const)` complete without
failures, including the importer assertions and the new symbolic-zero case.
The earlier full-suite run is not repeated for these review changes.
