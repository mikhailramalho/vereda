# A persistent map of known bytes

Recorded 2026-11-23.

On `esbmc/array_loop_summary_decrement_fail` (100000 iterations of
`a[i] = j`) the walk took about 26 s where ESBMC's symex takes 7.1 s with
`--no-summarize-array-loops` (#66). State kept its known bytes in a
`std::map` behind a `shared_ptr`, copied on write: each pass through the
unrolled loop shared the state between the body and the exit, and the
body's store then copied the whole map, up to `MaxKnown` (4096) entries.
Copying and freeing those maps took more than half of the walk.

The known bytes, and the objects with arrays of their own, now live in
`CellMap` (`src/memory/cellmap.h`), a persistent big-endian Patricia trie
on the 128-bit key object:offset. Copying a state copies a pointer; a change
rebuilds only the path to its key. `llvm::ImmutableMap` was the first
candidate, but an AVL tree's shape depends on the order of its insertions,
so two maps with mostly the same bytes need not share subtrees, and the
join would still walk both in full. A Patricia trie's shape depends only on
its keys, so the join (`Memory::ite`) asks `CellMap::diff` for the keys whose
cells differ and skips every subtree the two sides share; the arrivals at a
loop's exit, each a few bytes away from the previous one, then merge in
time proportional to those few bytes. Key order is in-order traversal, and
an object's keys are one subtree, which `flush` and `fillObject` use.

Same machine, both built the same way, base origin/main (b412c578):

| | walk before | walk after | walk peak memory |
|---|---|---|---|
| array_loop_summary_decrement_fail | 26.0 s | 9.2 s | 938 MB, unchanged |
| array_loop_summary_counter_fail | 25.9 s | 9.9 s | 940 MB, unchanged |
| array_loop_summary_store | 25.0 s | 8.5 s | 850 MB, unchanged |
| array_loop_summary_stride | 13.2 s | 4.5 s | 468 MB, unchanged |
| all 42 array_loop_summary tests | 181.9 s | 63.8 s | |

No verdict changed in the family, solve times did not move (the decrement
test still spends about 27 s in Bitwuzla on its 100000 array stores), and
`tests/bench.py` shows no regression (counter_fail 38.7 s to 19.1 s wall).
The walk's peak memory is the terms, not the map copies. What is left of the
walk is building terms, spread thin in a profile. Raising `MaxKnown` to
65536 changed neither walk nor solve on these tests, so it stays at 4096.
