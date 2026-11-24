# IR preparation on btor2c's big tasks: dead nondet locals cost mem2reg

Recorded 2026-11-24.

IR preparation took about 50 s on `btor2c-lazyMod.cal182` (ReachSafety,
expected TRUE) and on its sibling `btor2c-eagerMod.cal182`. Their `main` is
one function of 45,000 lines of C: at -O0, 39,860 allocas, 13,500 blocks and
420,000 lines of IR. `perf record -g` on `vereda --dump-ir-only` put 95% of
the time in mem2reg, inside `IDFCalculator::calculate`, called by
`PromoteMem2Reg::run`.

The cost was ours. `nondetLocals` gives each scalar local a store of a fresh
nondet value right after its alloca, in the entry block, so that a read
before any write sees one value rather than undef. That store makes a local
that clang stores once (`SORT_1 var_10 = ...;`) a local with two defining
blocks, and for such a local mem2reg computes the iterated dominance frontier
of its definitions, walking the dominator subtree of each one; from the
entry block that subtree is the whole function. One walk over 13,500 blocks
per local, for 40,000 locals.

On these tasks no local is read before it is written: each load follows a
store on every path to it, so the nondet value is dead. `nondetLocals` now
leaves alone a scalar local whose only users are simple loads and stores of
its own type, each load dominated by one of the stores (one `DominatorTree`
per function, built before anything is inserted). A local read
uninitialised on some path, one whose address escapes, or one accessed at
another type keeps its nondet store as before. A skipped local's nondet
value was never read, so the formula's meaning does not change;
`dropUninitialisedRecords` already removed its debug record, so neither does
the trace. Allowing several stores matters here: lazyMod writes
`var_737 = var_737 & mask_SORT_1;` after the declaration, 2,897 such locals in
cal182, which a single-store test left with their nondet store (7 s instead
of 3.5 s).

IR preparation time (`build/vereda --dump-ir-only` on clang -O0 -g bitcode,
RelWithDebInfo build, LLVM 21.1.2, one run each on a shared machine; reruns
under heavier load came out up to 25% slower on both sides):

| task | lines of C | before | after |
|---|---|---|---|
| btor2c-lazyMod.cal182 | 44,959 | 50.8 s | 3.5 s |
| btor2c-eagerMod.cal182 | 45,619 | 49.3 s | 4.4 s |
| btor2c-lazyMod.Heap | 2,878 | 0.28 s | 0.10 s |
| btor2c-lazyMod.vis_arrays_vsaR_p10 | 1,287 | 0.07 s | 0.04 s |
| btor2c-lazyMod.anderson.3.prop1-back-serstep | 1,617 | 0.10 s | 0.08 s |
| ldv-linux-3.14 cx88-cx8800.cil.i | 19,062 | 0.14 s | 0.13 s |

The cost grows with locals times blocks in one function, so only btor2c's
biggest tasks paid it in full. What is left on cal182 is LLVM's own: by
`perf`, mem2reg on the program's locals that are written more than once (its
state and input variables) about 40%, InstCombine 35%, reading the bitcode
15%.

The prepared IR differs only by the dead calls: in the IR before the change
none of the 39,858 nondet calls in cal182's `main` had a use, and the IR
after has as many phis and as many of each other instruction. The walk gains
too, since the walker made a symbol for each of those calls: on cal182 with
`--incremental-bmc` the first bound walked 43,761 symbols before and 3,902
after, each further unwinding adds about 6,880 as before, and bounds 1 to 27
report the same results (each too small: the task is TRUE, which plain BMC
does not prove).
