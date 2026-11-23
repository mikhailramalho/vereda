# Bitwuzla abstraction for Problem14

Recorded 2026-11-22.

Bitwuzla abstracts multiplication, division and remainder and refines them
lazily (option `abstraction`, on by default in 0.11), but only for terms
of 33 bits or more (`abstraction-bv-size`). Problem14's arithmetic is 32
bits. Setting the size needs camada's `configureOptions` hook, which is on
master but not in a release (0.23 has no hook and bundles Bitwuzla 0.9.1),
so Vereda was built against a local static camada 0.24 with Bitwuzla 0.11.0.

| Run | Problem14 k = 1, assertion check |
|---|---|
| Bitwuzla CLI on the dumped formula, defaults | out of memory (15 GB) at 181 s |
| CLI, abstraction off | out of memory at 150 s |
| CLI, `abstraction-bv-size=32`, property asserted | unsat, 111 s, 2.9 GB |
| CLI, `abstraction-bv-size=32`, property as an assumption | unsat, 62 s, 2.8 GB |
| Vereda, size 32 | timeout (900 s), 6.4 GB |
| Vereda, size 32, `pp-normalize` off | timeout (900 s) |
| Vereda, defaults | out of memory |

The CLI solves the same formula that Vereda times out on. Verbose output
shows the difference is in preprocessing: inside Vereda the rewrite pass
doubles the term graph and normalization then spends 36 s growing it to
2.3M nodes, while on the re-parsed dump both passes leave it unchanged. The
formula is the same; the term ids differ (the parser creates terms in
another order, and Vereda's term manager also holds the terms the walk
built and dropped), and Bitwuzla's rewriting orders operands by id. So
the CLI result depends on term order and is not something Vereda can rely
on.

The setting also costs elsewhere: uthash k = 2 solving goes from 40 s to
71 s, array12_pattern from 45 s to 54 s. Problem08, Problem102 and
SpamAssassin are unchanged, and the 189-task verdict check matches.

Not adopted. Problem14 likely needs the old behaviour back for this shape: a
global written under many branches was a store into the byte array, which
Bitwuzla bit-blasted only where a read needed it.
