# Small non-power-of-two table periods

Extend the periodic finite-table matcher to periods 3 through 15 after the
existing period-2/4/8 matcher. Keep the cheap low-bit encoding for those existing
periods. Require at least 128 elements for the new unsigned-remainder encoding
and verify every element against the candidate pattern before accepting it.
The bounded search is linear in table size with a fixed maximum period.

Use ceil(log2(table size)) index bits before unsigned remainder. This is exact
for all indices within the table. Memory::flushObject already guards the compact
function with the original full-width index bound and returns the original
default value outside the table, so narrowing does not change out-of-range
array behavior. Pattern elements can be symbolic; equality matching uses the
existing canonical terms.

Build with LLVM 23.1.6 and native rustc 1.100.0. The targeted CTest run has three
passes and one known-bug skip: table_patterns takes 0.18 seconds,
tuple_const_array_struct_member passes in 0.25 seconds, and
tuple_const_array_member_reads_fail passes in 41.13 seconds. The passing
member_reads fixture still exceeds the 4 GiB limit after 117 seconds. Its mixed
struct-field representation is tracked separately in #272. These are local head
measurements, not a controlled base/head speedup comparison.

Expand the unit comparison to every period from 2 to 15, across element widths
8/16/32/64, symbolic patterns, 257-element tables with incomplete final periods,
and a mismatching last element that must reject compression. The implementation
adds no flags and completes the periodic-matching scope of #273.
