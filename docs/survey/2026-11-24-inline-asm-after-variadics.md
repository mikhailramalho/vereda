# Inline asm on the LDV tasks once variadic calls are walked

Recorded 2026-11-24.

`2026-11-24-inline-asm.md` measured the asm rule of #125 on a main that
stopped most LDV tasks at their first call to a variadic function (printk,
dev_err). #122 walks those calls, so more tasks reach asm. Measured again
with the same 60 s limit on all 2,358 LDV unreach-call tasks: before is main
at 1b9b4bc8 (#122), after is that commit with #125 on top.

Before, 1,147 tasks give UNKNOWN on an asm call and 35 on llvm.bswap; 1,012
time out. Of the 1,147 stopped by asm, after:

| after | tasks |
|---|---|
| timeout (36 of them killed shortly before 60 s) | 723 |
| UNKNOWN at an asm that may write memory | 251 |
| UNKNOWN at an intrinsic (llvm.prefetch, llvm.ctpop, ...) | 83 |
| UNKNOWN at a struct of a pointer and an integer (two asm outputs) | 50 |
| UNKNOWN at other constructs | 24 |
| SUCCESSFUL, expected true | 3 |
| FAILED, expected false | 2 |
| FAILED, expected true | 11 |

Of the 35 stopped by llvm.bswap, 23 time out, 6 are killed shortly before
60 s and 6 stop at another construct. Of the 1,012 tasks that timed out
before, 1,011 still do (7 of them killed shortly before 60 s), and one now
stops at a variadic call.

The 11 wrong FAILED results come from writes through pointers that may point
anywhere. In 9 of them the write is a kernel atomic or bit operation, which
#125 turns into an atomicrmw: through `current` (`set_current_state()`, an
xchg; `current` is a per-cpu read, so a nondet pointer), or through a field
of a driver struct that the environment model allocated without filling
(`__set_bit`, `clear_bit` on `hdev->...`). With the atomics left refused,
these 9 give UNKNOWN; the other 2 still give FAILED. A nondet pointer may
point to any object, the LDV model's lock counters included, which is how
main already reaches its 51 wrong FAILED results on these tasks (against 19
right ones): the asm rule exposes more such writes but does not add the
cause. Giving a pointer that comes from outside the program an object of its
own would remove them; it is not built here.
