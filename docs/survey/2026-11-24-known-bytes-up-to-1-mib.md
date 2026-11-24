# Known bytes up to 1 MiB

Recorded 2026-11-24.

Issue #99. `docs/survey/2026-11-24-the-decrement-solve-against-esbmc-s.md`
found that raising `MaxKnown`, the number of known bytes a state keeps before
all go to the arrays, from 4096 to 1 MiB wins on its own (github_123, three
array_loop_summary tests without summaries, two overflow tests) but loses in
three places that cost time per known byte. `docs/survey/2026-11-23-a-persistent-map-of-known-bytes.md`
had kept 4096 because 65536 changed nothing on the tests it ran. This note
records the fixes for the losses and the numbers with them; the descending
flush of that note is not part of it.

All runs: Release builds of main (ac2604b1) and of the branch, same
mimalloc, `tests/bench.py --list` side by side (120 s limit, 4 GB of address
space per run) unless a table says otherwise. The machine has 80 cores and
ran other jobs (load 15 to 40); compare numbers within a row.

## Where the losses came from

**A join against a side that flushed an object.** `Memory::ite` made a
choice per byte that differs, reading a byte known on one side only from the
other side's arrays (`Terms::read`, up to 256 steps). When that side had
flushed the object, each read walks the flush's stores. On 26_exStbKey with
`MaxKnown` at 1 MiB, 1.47 M of the walk's 1.48 M `Terms::read` steps came
from two such joins, which read the 1552 and 1568 bytes of two objects one
by one; joins took 0.25 s of the walk against 0.08 s on main. In
array_loop_summary_store_fail without summaries the same happens to the
400000 bytes of `a` at the join of `j == 4 ||`. In
struct_array_ite_new_index_fail both sides know the 32768 bytes of `w`, and
a choice per byte solves far slower than main's choice of arrays.

**`Memory::pointers` at each call without a body.** It looked up the
candidate objects of the provenance of every known byte. On 26_exStbKey (13
such calls, about 4700 known bytes, up to 3000 of them parts of pointers) that
is one `candidates()` call per byte for 190 to 380 distinct (object,
provenance) pairs. On 23_picosat-846_01 (8 calls, 11448 known bytes) pointers
copied byte by byte give each byte a provenance of its own, read from memory,
whose candidates are every exposed object: `pointers` built that list for
each byte and inserted 15.6 M object ids into the sets it returns (7.2 M on
main), 0.44 s against main's 0.17 s.

## The changes

- `MaxKnown` is 1 MiB.
- `Memory::ite` first counts the differing bytes of each object, and moves an
  object with more than `MaxChoices` (1024) of them to the arrays on both
  sides before it reads any byte; the rest is as before.
- `Memory::pointers` looks each provenance up once, each (object, provenance)
  pair of the known bytes once, and adds the exposed objects once per object
  that holds a pointer read from memory. `candidates()` takes an optional
  `bool *read` for this: given, it leaves out object 0 and the exposed
  objects and says whether the pointer was read from memory. The sets
  `pointers` returns are the same as before.

## Choosing `MaxChoices`

The first version counted all differing bytes, 4096 at most; a scratch build
then took a second limit on bytes known on one side only from an environment
variable, with `pointers` already looking each provenance up once. Walk or
wall seconds, base → head, side by side:

| Limit | 26_exStbKey walk | struct_array_ite_new_index_fail wall | store walk | store_fail walk |
|---|---|---|---|---|
| `MaxKnown` 1 MiB only | 0.53 → 1.36 | 17.50 → timeout | 5.10 → 13.47 | 3.35 → 11.66 |
| all bytes, 4096 | 0.53 → 0.67 | 18.30 → 17.18 | 3.25 → 3.64 | 4.19 → 5.16 |
| one side 1024, all 4096 | 0.50 → 0.53 | 18.19 → 16.98 | 3.40 → 3.94 | 3.73 → 4.29 |
| one side 256, all 4096 | 0.50 → 0.53 | 17.58 → 17.14 | 3.35 → 3.82 | 3.68 → 5.00 |
| one side 256, all 1024 | 0.49 → 0.53 | 18.28 → 17.75 | 3.40 → 3.62 | 3.77 → 4.12 |

(store and store_fail run out of memory in the solver either way.) A limit
of 4096 leaves 26_exStbKey's objects of 1552 and 1568 bytes to be read byte
by byte; 1024 and 256 do the same everywhere. A build that printed the
largest number of differing bytes of one object at any join, run on every
test file that gives a verdict for 20 s each with its own flags, found
fourteen tests past 256 and five past 1024: 26_exStbKey (1568), lib_ctype
and lib_hidden (1536, known on one side only), struct_array_ite_new_index
and its `_fail` (32768); the no-summary tests are not in the suite.
lib_ctype, lib_hidden, 25_git-remote-gitkrb5, 31_cdaudio, github_2312_9,
null_offset_int_read, summary_start_refused, 08_adpcm_encode_nopointer and
scanf_tail_beyond_cap_fail took main's time with one-side limits of 1024
and 256. No test has between 1024 and 4096 bytes that differ on both sides,
so one limit of 1024 on all differing bytes behaves as "one side 1024, all
4096" on every test, and is what the branch keeps.

## Choosing `MaxKnown`

The branch built with `MaxKnown` from an environment variable; wall seconds,
one run each, 60 s limit:

| `MaxKnown` | array_loop_summary (nosum) | _nondet (nosum) | github_123 | overflow_09_bcnt | overflow_22_wordplay |
|---|---|---|---|---|---|
| 4096 | timeout | timeout | timeout | 0.70 | 1.00 |
| 64 Ki | timeout | timeout | 0.50 | 0.20 | 0.70 |
| 256 Ki | timeout | timeout | 0.50 | 0.20 | 0.70 |
| 512 Ki | 4.81 | timeout | 0.50 | 0.20 | 0.70 |
| 1 Mi | 4.81 | 9.91 | 0.60 | 0.20 | 0.70 |
| 2 Mi | 4.91 | 9.91 | 0.50 | 0.20 | 0.70 |

array_loop_summary fills an array of 100000 ints (400000 bytes) and _nondet
two; 1 MiB is the smallest power of two that keeps both known, and 2 MiB
gains nothing.

## Before and after

134 tests: the 15 that the earlier scan found past 4096 known bytes, the 9
above with joins past 256, the 41 with the longest walks in main's last CI
run, the 42 array_loop_summary tests with summaries and the 34 that give a
verdict without (`nosum/`, each with `--no-summarize-array-loops` added).
The rows that changed or that the losses concerned:

| Test | Walk | Solve | Wall | Peak MB | Verdict |
|---|---|---|---|---|---|
| github_123 | - → 0.09 | - → 0.00 | - → 0.51 | - → 68 | TIMEOUT → SUCCESSFUL |
| overflow_09_bcnt | 0.08 → 0.06 | 0.54 → 0.00 | 0.82 → 0.29 | 58 → 50 | SUCCESSFUL |
| overflow_22_wordplay | 0.06 → 0.04 | 1.51 → 0.69 | 1.88 → 1.06 | 127 → 113 | FAILED |
| nosum/array_loop_summary | 9.05 → 4.58 | - → 0.00 | - → 4.73 | 1214 → 901 | TIMEOUT → SUCCESSFUL |
| nosum/array_loop_summary_fail | 9.33 → 4.68 | 0.00 → 0.00 | - → 5.56 | 1205 → 901 | TIMEOUT → FAILED |
| nosum/array_loop_summary_nondet | 18.22 → 9.55 | - → 0.00 | - → 9.72 | 2001 → 1375 | TIMEOUT → SUCCESSFUL |
| nosum/array_loop_summary_nondet_fail | 11.53 → 7.41 | - | - | 1363 → 1191 | TIMEOUT → out of memory at 20 s |
| 26_exStbKey | 0.52 → 0.57 | 0.00 → 0.00 | 0.69 → 0.75 | 74 → 80 | SUCCESSFUL |
| struct_array_ite_new_index_fail | 0.98 → 0.77 | 17.17 → 16.42 | 18.50 → 17.52 | 769 → 766 | FAILED |
| struct_array_ite_new_index | 1.26 → 0.93 | - | - | 161 → 171 | TIMEOUT |
| 23_picosat-846_01 | - | - | 1.30 → 1.33 | - | UNKNOWN |
| nosum/array_loop_summary_store | 3.09 → 3.67 | - | 49.48 → 49.85 | 566 → 660 | out of memory |
| nosum/array_loop_summary_store_fail | 3.17 → 3.54 | - | 49.21 → 50.21 | 565 → 660 | out of memory |
| nosum/array_loop_summary_counter | 2.46 → 2.48 | - | - | 509 → 586 | TIMEOUT |
| nosum/array_loop_summary_decrement | 2.34 → 2.52 | - | 31.79 → 32.13 | 498 → 575 | out of memory |
| nosum/array_loop_summary_stride | 0.90 → 0.93 | - | 44.91 → 43.40 | 216 → 255 | out of memory |

Every other test kept its verdict and moved by less than 0.1 s. The eight
FUTURE array_loop_summary tests that need flags Vereda lacks give no verdict
with either binary. `ctest -L vereda` passes (322 tests), and so do the 2786
tests of `ctest -R 'esbmc.*(array|struct|memory|overflow|github)'`, whose
verdicts match those of main's last CI run but for github_123, promoted to
CORE, and the KNOWNBUG struct_array_ite_new_index, which runs out of time
here with both binaries and out of memory in CI. The earlier scan of the
840 KNOWNBUG and FUTURE tests that give a verdict found github_123 the only
one to flip; the tests the join limit touches are listed above.

## What is still slower

The no-summary tests that write a large array at constant indices and then
read it at a symbolic one (store, store_fail, counter, decrement, stride)
peak 40 to 95 MB higher and walk up to 0.6 s longer: their bytes now sit in
the persistent map, at about 200 bytes each, until the symbolic read moves
them to the arrays in one flush, where main moved them 4096 at a time. All
of them run out of time or memory in the solver either way. 26_exStbKey's
walk is 0.05 s longer. `pointers` still visits every known byte at each call
without a body, now up to 1 MiB of them; main visits every store in the
arrays instead, so a call costs time in the bytes written either way.
