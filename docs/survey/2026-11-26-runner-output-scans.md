# Reduce regression-runner scans of large progress logs

`vereda/mem_store_chain_reads` writes and reads 270,000 elements with array
loop summaries disabled. It produces about 79 MB of output with 540,000
unwinding lines. The verifier walks in about nine seconds, but repeated regex
scans of this output made the complete test take about eighteen seconds.

Keep all output and arbitrary EXPECT/EXPECT-NOT matching. Optimize metadata
extraction instead: find literal `VERIFICATION ` candidates before applying
the unchanged anchored verdict regex, and replace the OOM regex with checks
for its five exact literal strings. Preserve newline anchors, Unicode word
boundaries, multiple verdicts and last-verdict selection. Reuse the parsed
verdict in the runner, and skip recorded-verdict comparison for CORE tests.
`bench.py` uses the same verdict and OOM readers; its existing precedence
rules stay unchanged.

Capture elapsed execution time immediately after `execute()` returns, before
OOM classification, verdict parsing or expectation matching. Pass that elapsed
time to `record()` instead of having it read the clock later. This intentionally
changes REGRESS_STATS wall time to frontend/verifier execution plus output
capture, excluding subsequent output analysis. CTest still measures the whole
test process. Timeout records retain null wall time. Walk, solve and peak
memory extraction are unchanged, including incremental-bound totals.

## Measurements

Three runs per runner, alternating order, using the same existing verifier
binary reporting commit `5ab7bd46` (LLVM 23.1.6, camada 0.25 / Bitwuzla 0.11.0),
the same frontend, Clang and fixture, a 90-second timeout and 4096 MB limit.
All six runs report SUCCESSFUL. Only the runner changes.

| Metric | Base median | Updated median |
|---|---:|---:|
| Entire runner wall | 18.279 s | 12.592 s |
| REGRESS_STATS wall | 12.959 s | 9.501 s |
| Verifier walk | 9.254 s | 9.036 s |
| Verifier solve | 0.000 s | 0.000 s |
| Peak memory | 1154 MB | 1155 MB |

Complete test duration improves by about 31%. The smaller recorded wall time
also reflects its corrected measurement boundary; do not interpret that column
as a verifier speedup. The small walk difference is run-to-run variation with
an unchanged binary. Remaining walk/solve regex scans and arbitrary expectation
matching still cost time on large logs; they are outside this focused change.

## Validation

The output-reader checks compare the new verdict reader against the original
regex across line positions, CRLF, suffixes and Unicode boundaries, including
a long progress log and multiple statuses. OOM spelling checks preserve the
old regex's exact case sensitivity. Mixed legacy/current timing tags and
incremental totals remain covered.

All 17 configuration tests pass, including independent RUN processes,
timeouts, KNOWNBUG/FUTURE promotion, memory errors overriding printed verdicts,
and the near-memory-limit heuristic's generic VERIFICATION prefix rules.
A controlled clock test adds ten seconds during output classification and
verifies that recorded wall remains the two-second execution duration.

The updated runner also executes all 546 Vereda fixtures (560 RUN
configurations) against the unchanged verifier with no failures and one
existing expected skip. The full imported ESBMC suite was not rerun locally.
