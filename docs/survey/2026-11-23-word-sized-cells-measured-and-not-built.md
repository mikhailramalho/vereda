# Word-sized cells, measured and not built

Recorded 2026-11-23.

Issue #72 proposed keeping one cell per aligned word store instead of one
per byte, so that `a[i] = j` on an `int` array is one array store and the
read of `a[x]` one select instead of four. Before building it, two
measurements at e66d3d20 (with the persistent map of #70), on a machine also
running a baseline, so absolute times are high but comparable within each
table.

The array_loop_summary shape, 100000 iterations of `a[i] = j` and a read of
`a[x]`, with the element type changed: a 1-byte array has exactly the store
count and the single select word cells would give a 4-byte one.

| Element | Array stores | Walk | Solve | Peak |
|---|---|---|---|---|
| 8 bytes | 800000 | 30.0 s | 52.0 s | 4.7 GB |
| 4 bytes | 400000 | 15.0 s | 38.8 s | 2.6 GB |
| 2 bytes | 200000 | 7.3 s | 21.2 s | 1.6 GB |
| 1 byte | 100000 | 5.8 s | 16.5 s | 1.0 GB |

So word cells would cut this family's walk about 2.6 times and its solve
about 2.4 times, and still leave a 16.5 s solve where ESBMC takes about 2 s:
the cost left is the chain of 100000 stores on one array with 96-bit
indices, which loop summaries (#36) remove and per-object arrays (#71)
shorten and narrow.

Outside that family the bytes are not what costs. In the first ReachSafety
baseline Vereda spent about 11000 s solving and 450 s walking. The 30 Arrays
and Heap tasks with the most solve time (all timeouts but one: array-tiling,
array-crafted/zero_sum, array-fpi, array-patterns) have small formulas at
`--unwind 8`: 0 to 1592 array stores, 35 to 1442 selects, 16 to 263 KB of
SMT-LIB; half of them have no array store at all, every element staying in
the known-byte map. Cutting those counts by four does not touch what makes
them hard: sums and remainders over many symbolic elements, at bounds
climbing to 50. The tasks with the most walk time do not use word memory
either: `loops/heavy-1` and `heavy-2` loop over a `char` array, and
`nested5-1` has no array.

Word cells would also be the largest change to the memory model: a split
rule for every narrower, unaligned or partial access, for byte-wise copies
and fills of part of a word, and for joins of a word cell with byte cells,
plus provenance per width. For a gain confined to a test family that #36
removes outright, it is not built; revisit if a real task shows a long
chain of word stores that a summary cannot replace.
