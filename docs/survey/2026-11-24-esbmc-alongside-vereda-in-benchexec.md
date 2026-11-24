# ESBMC alongside Vereda in BenchExec

Recorded 2026-11-24.

`.github/workflows/benchexec.yml` can now run ESBMC on the same tasks as
Vereda, so that #12's comparison comes from one run on GitHub's runners
rather than from ESBMC numbers taken another day on our own machine. With
the `tools` input set to `vereda esbmc`, each shard's job runs Vereda's
BenchExec and then ESBMC's, both two tasks at a time with 2 cores, 6 GB
and 60 s, so the two tools' numbers for a task come from the same runner.
The default stays `vereda`: ESBMC doubles a run's time, and most runs only
look for Vereda's wrong or lost verdicts.

## How ESBMC runs

The `esbmc` input names a release tag of github.com/esbmc/esbmc (default
v8.8, the latest). The job takes the release's `esbmc-linux.zip` (300 MB;
one statically linked binary of 822 MB with its debug information, 165 MB
stripped, which needs no library from the runner) and the SV-COMP wrapper
`scripts/competitions/svcomp/esbmc-wrapper.py` from the same tag, which
for a C task needs only Python's standard library. Fetching both takes
8 s. The asset has had the same name since v8.3.

BenchExec 3.37 ships an `esbmc` tool-info module, which `esbmc.xml` uses.
It runs `esbmc-wrapper.py` from the tool directory, in that directory,
with `-p <property file>`, `--arch 32` or `64` from the task's data model,
the run definition's options (`-s incr`, incremental BMC, as ESBMC's
SV-COMP entry and our local baselines run it) and the input file. The
wrapper runs `./esbmc` with the competition's options and prints the
verdict; the module takes a `FALSE_*` kind if any output line names one,
else `FALSE` or `TRUE` anywhere in the output (the wrapper lowercases those
words in ESBMC's own output so that only its verdict carries them), and
otherwise TIMEOUT when the run timed out and ERROR unless the output says
`Unknown`. The module gets the version by running `esbmc-wrapper.py -v`,
which runs `./esbmc --version` from the current directory, so ESBMC's
BenchExec starts in the tool directory; from elsewhere the version is
empty and BenchExec only warns. The witness the wrapper writes lands in
the overlay over /home and BenchExec keeps it next to the results.

BenchExec, started alone in an empty cgroup, moves itself into a child
cgroup and enables every controller for the children of the one it was
given, where no process may then join. A second BenchExec in the same job
therefore needs a cgroup of its own: the job makes `benchexec-vereda` and
`benchexec-esbmc`.

The planner sizes shards as before, at most 4.5 hours each even if every
tool ran every task to its time limit, so with both tools Loops takes 3
shards, Arrays 2, ControlFlow and Heap 1 each.

## The report

For each set and tool, the job summary gives correct and wrong TRUE and
FALSE, unknown, timeout, out of memory and other (an error) counts, the
SV-COMP score (+2, +1, -32, -16) and the CPU time; on the tasks both tools
answer correctly, each tool's total and median CPU time and peak memory
and on how many each is faster and smaller; and each tool's wrong
verdicts. Only a wrong verdict of Vereda's fails the job. The comparison
with an earlier run stays Vereda's. table-generator's table has a column
per tool (and one for the earlier run's Vereda), with a diff of the tasks
whose results differ.

BenchExec measures memory as the peak of the run's cgroup, which counts
the pages the run itself brought into memory. Both tools' files are
written to the runner when the job installs them, so their pages are
already cached, charged to the job's own cgroup: the peak leaves out the
binaries and shared libraries and counts what each tool allocates. The
first runs of each tool show no more than the later ones (Vereda 19 MB on
`loops/array-1`). This is why Vereda's median below is about 20 MB where
#169's `/usr/bin/time`, which counts every resident page, gave 90 MB.

## Test runs

A filtered run, the 63 tasks of `^loops/` in Loops
(https://github.com/mikhailramalho/vereda/actions/runs/45424808705, Vereda
8f2b8df1 with this change, ESBMC v8.8.0): Vereda 11 min, ESBMC 11 min in
one job on an Intel Xeon 6975P-C runner.

| Tool | Correct TRUE | Correct FALSE | Wrong | Timeout | Out of memory | Score |
|---|---|---|---|---|---|---|
| Vereda | 19 | 23 | 0 | 20 | 1 | 61 |
| ESBMC | 18 | 23 | 0 | 22 | 0 | 59 |

On the 41 tasks both answer, Vereda takes 4 s against 23 s (median 0.07 s
against 0.16 s) and is faster on all 41, with a median peak of 20 MB
against 49 MB, smaller on 40.
