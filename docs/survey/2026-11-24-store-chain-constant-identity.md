# Constant identities in store-chain reads

Recorded 2026-11-24. Issue #183.

The earlier profile in `2026-11-24-terms-read.md` predates #197's term
handles and #203's larger known-byte map. It described a hash lookup in
`values` for each store index. On main at d8b2e287, `Terms::constant` gets a
bit-vector constant directly from the tagged term handle; there is no
`values` lookup. The `nodes` lookup remains. The known-byte limit is now
1 MiB, so more writes stay out of the store chain too.

I tried passing the original constant `Term` through `Terms::read` and
comparing it with each constant store index by identity instead of comparing
their `APInt` values. Bit-vector constants are interned by value and width;
boolean constants are shared too. The prototype keeps the same store-chain
limit and join handling, and still checks whether each index is constant.
It does not add a node flag or a per-array index table.

## Measurement

Release builds, LLVM 21.1.2, camada 0.24 and mimalloc, on the same machine.
Each run uses `--no-summarize-array-loops --dump-smt-only` and records the
walk time and walk's peak memory reported by Vereda. Base and prototype run
alternately, reversing order on the second pair. The dumps are compared by
SHA-256. The times do not include solving.

| Test | Main walk s | Prototype walk s | Main peak MB | Prototype peak MB |
|---|---|---|---|---|
| array_loop_summary | 2.521, 2.542, 2.568 | 2.705, 2.569, 2.563 | 580–582 | 580–581 |
| array_loop_summary_decrement_fail | 1.714, 1.632, 1.650 | 1.669, 1.719, 1.619 | 390 | 388–391 |
| array_loop_summary_nondet | 6.064, 5.907 | 6.294, 6.346 | 942 | 942 |
| github_302 | 39.282, 37.978 | 38.279, 37.919 | 560–562 | 569–572 |

The dumped SMT is byte-identical within each test across all runs. On these
tasks the change has no repeatable benefit; it is slower on the nondet case.
The original decrement profile had already found no measurable time in
`read` after #182.

Decision: do not keep the identity comparison. The alternative per-array
index table and a node flag were not built or measured, and the experiment
does not show whether either could help other store-heavy cases. The
prototype was checked on these four formulas only, not across the suite;
that wider comparison is still needed for a change that is kept.
