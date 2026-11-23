# First measurements

Recorded 2026-11-21.

Vereda at commit 191a09e, same limits. Vereda has no incremental mode yet:
a script runs `--unwind 1, 2, ...` as separate processes in one 900 s
budget, so lower bounds are re-walked at every step. Peak memory is the
process's, after the walk (walker plus the terms it built) and after
solving.

| Task | ESBMC, plain BMC | Vereda, bound reached | Walk peak | Solve peak |
|---|---|---|---|---|
| SpamAssassin-loop | out of memory at k = 3 | k = 20 | 83 MB | 474 MB |
| uthash_JEN_nondet_test1-1 | timeout at k = 1 | k = 2 | 2.0 GB | 2.2 GB |
| Problem14 | out of memory at k = 1 | k = 1 | 0.9 GB | 4.2 GB |
| Problem08_label00 | timeout at k = 6 | k = 2 | 4.1 GB | 5.4 GB |
| Problem102_label00 | timeout at k = 6 | k = 2 | 4.1 GB | 5.3 GB |

sanfoundry_02_ground and bAnd4 loop 100,000 times, so no bound short of a
full unroll says anything; their out-of-memory at k = 1 came from
`--goto-unwind`, which unrolls constant loops before symbolic execution.
At `--unwind 100001`, without `--goto-unwind`:

| Task | ESBMC | Vereda |
|---|---|---|
| sanfoundry_02_ground | symex 61 s, encoding 50 s, then out of memory at 15 GB | walk 99.97% done at the 900 s limit, 6.8 GB |
| bAnd4 | timeout, 482 MB | walk 99.98% done at the 900 s limit, 5.4 GB |

## What the numbers say

- Vereda never ran out of memory; its peak stayed under 7 GB. On the three
  tasks where ESBMC ran out of memory at a small bound, Vereda got further
  (SpamAssassin k = 20 against 3) or as far (Problem14 k = 1, uthash k = 2
  against a timeout at 1).
- ESBMC is ahead on the two ECA tasks (k = 6 against 2) and walks the full
  unroll of sanfoundry much faster.
- **The walk, not the solver, is the cost on the large tasks.** On the ECA
  tasks it takes 2.2 GB at k = 1 and 4 GB at k = 2 before the solver
  starts. These tasks keep their state in 23 `int` globals, read 244,000
  and written 127,000 times; in the one byte array each load is 4 selects
  and 3 concatenations, each store 8 array stores, and 262,000 branches
  merge both arrays at their joins. None of the globals has its address
  taken.
- **Walking slows down with depth.** About 0.7 ms per iteration at 8,000
  iterations of one loop, but some 400,000 iterations took over 15 minutes.
  A profile at 8,000 iterations puts nearly all the time in Bitwuzla's term
  construction (hash-consing, term export, rewriting) reached through
  camada; the walker's own code is under 1%. The likely cause, not yet
  confirmed, is selects over store chains that grow with every iteration,
  which the rewriter looks back through.

## Consequences for milestone 1

The core bet survives on memory but not yet on time. The fixes are the ones
the plan already lists, now with evidence for their order:

1. Folding as terms are built (plan, section 4.2), above all a load from a
   known store, so selects stop growing with the store chain.
2. Partitions (section 4.4), starting with globals whose address is never
   taken: each becomes a plain value, which removes almost all array work
   on the ECA tasks.
3. Incremental BMC (section 4.2), so a run stops re-walking lower bounds.

Not measured: Problem14 at k = 2. SeaHorn, SMACK and Symbiotic (plan,
milestone 0) were skipped. SeaHorn ships only as a Docker image, and the
Symbiotic (SV-COMP 2026) and SMACK (SV-COMP 2022) archives were not run
outside a container, so ESBMC is the only comparison here.
