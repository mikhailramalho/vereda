# ReachSafety baseline against ESBMC

Recorded 2026-11-23.

The first full run of milestone 1's measurement set: SV-COMP's Loops,
Arrays, Heap and ControlFlow, every task with an unreach-call verdict (1407).
Both tools ran incremental BMC with 60 s and 6 GB per task, each task in its
own systemd scope, 12 tasks per tool at a time on an 80-core machine. Vereda
was ade4de66 with `--incremental-bmc --property-file unreach-call.prp`, which
stops at bound 50. ESBMC was b12e1ead72 through its own SV-COMP wrapper,
`esbmc-wrapper.py -s incr`, so it had the competition's options (`--sv-comp`,
interval analysis, `--goto-unwind`, `--unlimited-k-steps`, ...). The scripts
and per-task results are outside the repository, in
`~/tools/baselines/2026-11-23-reachsafety`.

| Set | Tasks | Vereda correct | ESBMC correct | Vereda wrong | ESBMC wrong |
|---|---|---|---|---|---|
| Arrays | 440 | 94 | 128 | 1 | 0 |
| ControlFlow | 73 | 28 | 40 | 14 | 0 |
| Heap | 136 | 118 | 118 | 9 | 0 |
| Loops | 758 | 344 | 418 | 0 | 0 |
| All | 1407 | 584 | 704 | 24 | 0 |

Vereda's other results: 491 UNKNOWN, 302 timeouts, 6 crashes. ESBMC's: 688
timeouts, 15 out of memory.

On the 564 tasks both answer correctly, Vereda is faster on 502 (median
0.19 s against 0.48 s) and uses less memory on 557 (median 64 MB against
121 MB). Its total time is higher, 955 s against 885 s, because of a few slow
tasks. Vereda alone answers 20 tasks, on which ESBMC timed out (19) or ran out
of memory (1). ESBMC alone answers 140: on 108 of them Vereda gave up at
bound 50, after a median of 0.6 s, and ESBMC answered 88 of those within 10 s
by unwinding further. Letting Vereda keep raising the bound until its time
runs out, as ESBMC's `--unlimited-k-steps` does, is the largest gain in reach.

The 24 wrong verdicts became issues: the null object's base left free (#6,
#7, #8), uninitialised VLA reads (#9), and setjmp/longjmp walked as plain
calls (#10). The 6 crashes, a stack object larger than the 32-bit address
space, are #59. ESBMC's 37 wrong results in an earlier run came from copying
only part of its wrapper's options (no `--no-div-by-zero-check` among them);
through the wrapper it has none, as in its SV-COMP pre-run.

Two harness lessons. ESBMC's signal handler calls `killpg(0, sig)`
(`src/util/base/signal_catcher.cpp`), so the first task to hit its time limit
took down every task in the same process group; each task now runs under
`setsid`. And the wrapper swallows ESBMC's output when the kernel's OOM killer
stops it, so an out-of-memory run shows as a SIGTERM before the time limit,
not as a message.
