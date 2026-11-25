# Retaining inline bit comments in the console layout

Recorded 2026-11-25.

The user preferred binary values to remain inline comments, as in
`counter = 2; // 00000000 00000000 00000000 00000010`. This supersedes the
separate bits line in the earlier console-output note. State headings,
assumptions and the violated-property block remain plain text, and the log
tags and results summary are unchanged.

PR #235's initial CI run failed only esbmc/overflow-neg: its standalone binary
expectation still matched the old comment prefix. Retaining inline bit comments
also preserves that expectation. The importer and affected fixtures now use
the inline format, and the reader test additionally covers a bits-only pattern.

The branch merges origin/main at 330aed17. Its loop-location cache and
counterexample formatting optimizations are retained: progress uses the cached
location string, and trace values are deduplicated before direct formatting.
Only their presentation is adapted to the new layout.

The complete local run exercised 6,959 tests. Its only failures were stale
headings and summary expectations in the two solver tests added by main;
after updating those expectations both passed on rerun. overflow-neg passes
with the inline comments. The suite's expected skips remain skips.
