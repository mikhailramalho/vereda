# Partitions from a points-to analysis

Recorded 2026-11-23.

Should memory split into one array per alias class, chosen by a static
points-to analysis (plan, sections 4.3 and 4.4)? Measured at 948efd67 with
a build that counts each access and has two switches. "split" gives every
object its own arrays the first time the walk writes it at a constant
object, which is the most a partition can give with no points-to: an access
at an unknown object picks among all of them. "skip" makes a select step
over stores into another constant object, which is what partitions give
the solver; the 2026-11-22 section tried the same and saw no change.

How accesses reach memory today, over the 178 of the 189 tasks whose walk
ends (`--unwind 5`):

| Pointer at the access | Accesses | Share |
|---|---|---|
| Constant object and offset (known bytes) | 13,455 | 89 % |
| Constant object, symbolic offset | 1,336 | 8.8 % |
| One of two constant pointers | 302 | 2.0 % |
| Unknown object | 20 | 0.13 % |

Only an access at an unknown object needs a points-to set, and those come
from 2 tasks (tdh_sys_config, relax-2-2). An access at an unknown object
also flushes every known byte; that happens 9 times, all in those two
tasks. A static count on the prepared IR agrees: of 1,493 objects
(globals, allocas, heap calls), 78 have an address that is stored, turned
into an integer or passed to an external function, all in 8 tasks.

| Run | Verdicts | Walk + solve |
|---|---|---|
| 189 tasks, base | 2 FAILED, 171 SUCCESSFUL, 7 UNKNOWN, 9 out of time | 365 s on 172 tasks |
| 189 tasks, split | same, but parport runs out of time in the walk | 464 s |
| 189 tasks, skip | same as base | 404 s |
| 10_fir_new, 42 array_loop_summary tests, picosat; base | | 132 s on 26 tests |
| the same, split | same | 139 s |
| the same, skip | same | 132 s |
| uthash, `--unwind 2`, base | SUCCESSFUL | walk 13.8 s, solve 0.0 s |
| uthash, split | SUCCESSFUL | walk 35.5 s, solve 0.0 s |
| uthash, skip | SUCCESSFUL | walk 14.1 s, solve 0.0 s |

Two other agents shared the machine, so differences under about 15 % are
noise: skip's 404 s against 365 s is mostly noise. Split is slower on
every array_pattern task (array10 2.1 s to 4.0 s, array3_pattern_buggy
21 s to 37 s), as each join merges every separate object and the solver
gets more arrays. uthash, with 236 accesses at an unknown object, walks
2.6 times longer under split, since each read picks among about 8 objects.
uthash at `--unwind 5`, the fuzzles and picosat did not finish in 120 s in
any mode, so they give no numbers.

Where the time goes instead: the slow array_pattern tasks (12 to 24) each
have one object, a VLA. array12's formula is 72 KB, 281 selects over 152
stores into that one array, with products of symbolic values in the
indices and the stored values. 10_fir_new reads and writes only known
bytes. The array_loop_summary tests have one object each; their flushes
come from the cap of 4,096 known bytes, not from pointers. Partitions do
nothing for any of them.

ESBMC and CBMC go the other way: each object is its own SMT variable, and
symex keeps a value set per pointer, so a dereference becomes a choice over
that set (`src/pointer-analysis/README.md` in ESBMC). SMACK and SeaHorn use
a region per DSA node, which is the plan's model. Vereda's known bytes and
split pointers already give the common case of both: the object is
constant at 98 % of accesses, without a static analysis.

Decision: skip the points-to analysis and partitions for now. Recheck when a
task's time goes into accesses at an unknown object, which the counters
here show in one line per run. When that happens, the change is:

1. A field-insensitive Steensgaard pass on the prepared IR, after
   `linkModels`: union-find over allocas, globals and heap calls, joined
   through stores and loads of pointers, gep, phi, select and calls into
   defined functions; `inttoptr`, external calls and byte-wise copies of
   pointers join the class that points anywhere. A few hundred lines and
   linear time; field-sensitive Andersen (plan, section 5) only if that
   class is too coarse.
2. Memory gets one data and provenance pair per class in `State`, keyed by
   class instead of by object, as `State::separate` is now. A pointer's
   class comes from its IR value, so an access at an unknown object picks
   among the objects of one class, and a flush of known bytes empties only
   that class. Joins merge only the classes either side wrote.
3. The trace and the witness do not change: they read values, not arrays.

Cost: the pass, plus a merge per class at each join, which is what made
split slower here; it pays only where unknown-object accesses dominate.
