# Avoid recording array update history as lambda dependencies

Recorded 2026-11-27.

Issue #424 reports that `array-industry-pattern/array_mul_init` and
`array_of_struct_ptr_mul_init` started running out of memory after #395.
Previously they reached the time limit. The regression is in auxiliary walk
metadata, rather than the number of distinct lambda definitions needed.

## Profile

Instrumenting `Terms::uncached` on main `a45e5eb1` identifies `constrained`,
the set of array/index pairs already visited when constraining reachable
lambdas, as the growing table. Immediately before the allocation failure:

| Task | `constrained` pairs | `under` entries | Lambdas | Constant arrays | Term entries |
| --- | ---: | ---: | ---: | ---: | ---: |
| `array_mul_init` | 99,614,720 | 19,959 | 2 | 2 | 259,491 |
| `array_of_struct_ptr_mul_init` | 99,614,720 | 19,959 | 2 | 3 | 169,665 |

Each conditional update adds store/choice nodes above the same two initial
array lambdas. A read at a fresh index traverses the entire update history,
recording every intermediate array/index pair. N updates and N read indices
therefore produce quadratic metadata despite only two lambda ancestors.

LLVM 23's DenseMap grows at a three-quarter load factor. Near 100 million
pairs, the roughly 2 GiB bucket allocation needs to double to roughly 4 GiB,
while the previous allocation is still live. That cannot fit alongside the
rest of the process under a 6 GiB address-space limit, explaining the abrupt
failure at much less than 6 GiB resident memory.

## Change and correctness

Keep the dependency DAG introduced by #395, but forward through a child's
existing dependency list instead of always linking to the child array itself.
A store inherits its parent's list. A choice merges and deduplicates its two
arms' lists when their union fits in two entries. Otherwise it keeps the two
original arms, preserving the bounded representation for larger lambda sets.
The temporary union has space for four entries; the stored list always fits
in the existing two-entry inline storage.

This preserves the set of reachable lambdas: a store introduces none, and a
choice's set is the union of its arms' sets. Replacing an arm by its existing
cover preserves that union; deduplication does not remove any distinct lambda.
The larger-union fallback is exactly the previous DAG representation. SMT
array operations, lambda reductions and the constraints they require stay the
same; only the paths used to find those definitions change.

For a history sharing one or two lambdas, each new array points straight to
those lambdas instead of the preceding store or choice. Reads no longer
record all the intervening history. We do not clear or cap `constrained`:
that would leave repeated long traversals instead of removing their cause.
We also do not restore unbounded transitive dependency vectors, which were
the reason #395 introduced this DAG.

## Measurements

The fixture revision is sv-benchmarks `5092e58a00`. Builds use LLVM 23.1.6,
camada 0.25, mimalloc 3.7.0 and RelWithDebInfo on the Xeon E5-2698 v4 host.
Runs use ILP32, `--incremental-bmc --property-file unreach-call.prp` and a
6 GiB address-space limit. The instrumented baseline fails after 27.8 CPU
seconds / 3366 MiB peak RSS on `array_mul_init`, and 28.3 seconds / 3315 MiB
on the struct task. At bound 100001, the fixed walks take about 12.7 seconds /
1562 MiB and 9.4 seconds / 1049 MiB, with one check and 200000 symbols each.
Neither fixed prototype reaches one million cached pairs. Both survive a
310-second wall limit without an OOM, spending the remainder in the solver.

A second run of each task uses the clean binary directly on the saved LLVM
IR so the resource accounting includes the verifier even when a CPU limit
kills it. With a 300-second soft / 305-second hard CPU limit and 6 GiB of
address space, both head runs reach the CPU budget without an OOM or a
verification verdict:

| Task | Baseline outcome / CPU / peak RSS | Fixed outcome / CPU / peak RSS |
| --- | --- | --- |
| `array_mul_init` | OOM / 27.75 s / 3366 MiB | CPU-capped / 304.64 s / 3909 MiB |
| `array_of_struct_ptr_mul_init` | OOM / 28.34 s / 3315 MiB | CPU-capped / 304.08 s / 3357 MiB |

This restores the pre-#395 behavior reported in the issue: spending the time
budget solving instead of failing early while growing walk metadata. The
higher final RSS of the first fixed run is after substantially more solving.

The new construction regression initializes two arrays completely, performs
20000 conditional updates, and stops at the second loop's unwinding check.
The extra final iteration is empty, so every actual store remains in bounds.
Stopping there avoids the large nonlinear solve while keeping the arrays
live for the later property. Under a 4 GiB address-space limit, main fails
out of memory after 26.4 CPU seconds / 3238 MiB. The fixed ILP32 run reaches
the intended unwinding failure in 5.2 CPU seconds / 696 MiB. It also passes
in LP64 and is registered as two separate RUN configurations.

`vereda/lambda_dependencies` asks Bitwuzla to prove the selected value equals
an explicit scalar specification for one, two and four lambda ancestors,
nested choices, conditional stores, an opaque array arm, and another store
reusing the already-constrained index. Four ancestors exercise the bounded
DAG fallback. The full own suite has 723 passes and one existing skip.

The original #395 memory cases still return TRUE at bound 11:
`ddv_machzwd_inb` uses 11.3 CPU seconds / 737 MiB, and `loops/n.c24` uses
4.5 seconds / 801 MiB, with `-m32 --no-unwinding-assertions --property unreach-call`.

The original mul-init tasks still need solver improvements; #432 tracks
that separately. Growth involving genuinely different lambdas and indices,
including conditional copies, remains under #401.

Follow-up issues: #401, #432
