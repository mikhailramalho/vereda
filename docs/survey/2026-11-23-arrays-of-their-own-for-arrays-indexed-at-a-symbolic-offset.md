# Arrays of their own for arrays indexed at a symbolic offset

Recorded 2026-11-23.

Issue #71. An alloca or global that some getelementptr indexes at a
symbolic offset, and whose provenance does not escape (no pointer to it is
stored or passed on; LLVM's PointerMayBeCaptured with the Provenance mask,
so comparing its address does not count), gets data and provenance arrays of
its own from its allocation on, indexed by the offset alone at pointer
width. It starts from fixed nondet arrays, which also stand for its bytes on
a path that has not created it yet, so a join never meets the object in the
shared 96-bit arrays on one side. An access through a pointer whose object
is not constant still chooses among every object with arrays of its own:
`Terms::split` takes only an `ite` of two constants, so a pointer that may
be one of three arrays reaches memory as an unknown object.

Measured with the base, origin/main e66d3d20, and the head built the same
way, side by side on a loaded machine (`nice`, other jobs running):

| | base | head |
|---|---|---|
| array_loop_summary_decrement_fail, solve | 30.5 s | 26.6 s |
| the 42 array_loop_summary tests, walk / solve | 76 s / 610 s | 79 s / 606 s |
| whole suite (6633 tests, same verdicts), wall / solve | 2512 s / 851 s | 2375 s / 824 s |
| ReachSafety Arrays (440 tasks), correct / wrong | 99 / 0 | 99 / 0 |
| the 99 tasks both answer, time | 57.7 s | 56.5 s |

`tests/bench.py --stats` on the two suite runs found one test that changed:
esbmc-unix/05_pfscan-1.0_01 reaches its UNKNOWN (a calloc with a symbolic
size) in 11 s instead of 73 s. One Arrays task, array-lopstr16/break-1,
moved from UNKNOWN just under the 60 s limit to a timeout; unloaded it takes
47.8 s on base and 52.2 s on head.

The decrement test still has 400000 stores: the index is now 64 bits wide
instead of 96, and that is worth about a tenth of its solve time. The chain
length is what costs, so word-sized cells (#72) and loop summaries (#36)
remain the changes that matter there; this one pays where the stores of several
arrays would share one chain.
