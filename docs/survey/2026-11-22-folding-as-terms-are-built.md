# Folding as terms are built

Recorded 2026-11-22.

The first fix above, with the same tasks, bounds and caps (15 GB, 900 s).
Terms are now built through `encode/terms.h`, which shares equal terms and
folds constants, identities, an ite with a constant guard, extract over
concat, and operations on an ite whose branches are constants. A branch
whose guard folds to false is not walked. Bytes written at a constant
address stay in a per-state map and reach the byte array only when an
access at a pointer that may alias them needs it. A malloc result,
ite(ok, object, null), is split into one access per side, or one side when
the path guard already implies ok.

| Task | k | Walk before | Walk after | Solve before | Solve after | Peak before | Peak after |
|---|---|---|---|---|---|---|---|
| Problem08 | 1 | 14.9 s | 0.5 s | 5.2 s | 0.06 s | 2.4 GB | 0.7 GB |
| Problem08 | 2 | 27.9 s | 1.1 s | 190 s | 3.5 s | 5.4 GB | 0.7 GB |
| Problem08 | 3 | not reached | 5.3 s | | 28 s | | 1.8 GB |
| Problem102 | 2 | 28.1 s | 1.1 s | 76 s | 3.4 s | 5.3 GB | 0.7 GB |
| Problem102 | 3 | not reached | 5.3 s | | 28 s | | 1.7 GB |
| uthash | 1 | 49.5 s | 0.1 s | 15.2 s | 1.5 s | 1.2 GB | 0.2 GB |
| uthash | 2 | 260 s | 34 s | 115 s | 18 s | 2.2 GB | 1.0 GB |
| SpamAssassin | 12 | 0.03 s | 0.03 s | 20 s | 20 s | 0.2 GB | 0.2 GB |
| sanfoundry | 100001 | over 900 s | 61 s | | over 840 s | 6.8 GB | 3.4 GB |
| Problem14 | 1 | 5.0 s | 2.1 s | 569 s | out of memory | 4.2 GB | over 15 GB |

Every verdict is the unwinding assertion of loop 0 or 1, as before. On 170
small Arrays and reducercommutativity tasks and the 19 fetched spike tasks,
at `--unwind 5` without unwinding assertions, the verdicts match the build
before folding, except wc-2, which went from UNKNOWN to SUCCESSFUL: its
first loop runs 10 times, so at bound 5 nothing after it is reachable, and
folding now sees that instead of walking into a function it cannot handle.

Problem14 got worse. It runs 57,776 32-bit multiplications and remainders.
With byte arrays, Bitwuzla bit-blasted a stored value only when a read
needed it; with values, each global is an ite over every branch's
arithmetic and all of it is bit-blasted up front. Z3 also runs out of
memory on it. Bitwuzla's lazy abstraction of large arithmetic (option
`abstraction`) is the likely fix; camada does not pass solver options
through yet.

SpamAssassin is unchanged: its cost is arithmetic in the solver, not memory.
