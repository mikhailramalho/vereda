# Inline asm

Recorded 2026-11-24.

Every inline asm call gave UNKNOWN (issue #19). On SV-COMP's tasks it occurs
only in the LDV driver sets: no ReachSafety task of the 2026-11-23 baseline
(Arrays, ControlFlow, Heap, Loops) runs one, while of the 2,630 LDV files
(ldv-linux-*, ldv-commit-tester, ldv-consumption, ldv-challenges,
ldv-validator-*), compiled with clang, 1,657 hold one or more asm calls, 55,312 in all.
Counted by what this change does with them (files that hold at least one,
then asm calls):

| idiom | files | calls | rule |
|---|---|---|---|
| kernel atomics and bit operations (`lock; incl`, `decl; sete`, `bts`, `btr`, `bts; sbb`, `xchg`, `xadd`) | 1,225 | 5,160 | atomicrmw, exact |
| register outputs only, no "memory" clobber (`%gs:` per-cpu reads, `inb`, `bsr`, `call __get_user_N`, ...) | 1,218 | 17,856 | nondet outputs |
| `ud2` of `BUG()` | 1,096 | 11,435 | llvm.trap, path ends |
| empty, `mfence`, `lfence`, `sfence`, `rep; nop` | 891 | 9,989 | removed |
| may write memory, none of the above | 790 | 7,206 | UNKNOWN when reached |
| MMIO read (`readl`: mov from memory, "memory" clobber) | 609 | 943 | nondet output |
| MMIO write (`writel`: mov to memory) | 598 | 964 | volatile store |
| no outputs, no "memory" clobber (`outb`, ...) | 307 | 428 | nothing |
| `bswap` | 246 | 339 | llvm.bswap |
| `cmpxchg` against rax | 151 | 487 | cmpxchg, exact |
| empty with an output tied to an input (`OPTIMIZER_HIDE_VAR`, `RELOC_HIDE`) | 72 | 505 | the input |

The ones still refused are per-cpu writes (`incl %gs:...`, `addl %1,
%gs:...`: preempt counts and per-cpu statistics, a segment-relative address
that flat memory does not have), paravirt calls (`call *pv_irq_ops...` with a
"memory" clobber), `btc`, and a few others.

ESBMC drops every asm statement (`code_skipt` in its clang front end), so an
output keeps whatever value it had; that is the behaviour #19 asks to avoid.

The rule follows GCC's contract for asm: it writes its outputs and no other
memory unless it clobbers "memory". The idioms are recognised in `ir` on the
instructions an asm runs inline (labels, lock prefixes and the
`.pushsection`/`.popsection` bookkeeping dropped) and on the kind of each
constraint, and become exact IR: the kernel's atomics read, change and write
their operand, which in one thread is an atomicrmw, and an operand size
written on the instruction (`incl`, `${0:k}`) must be that of the C object.
`ud2` ends the path, as `__builtin_trap` does, only where the program expects
no return (`unreachable`, or the empty endless loop CIL makes of it), since
newer kernels' `WARN()` runs ud2 and resumes after it. `readl` loses its
"memory" clobber, which is only a compiler barrier, and gives any value, as a
device register may hold anything. An asm with alternatives (code the kernel
patches in at boot) or in Intel syntax is not an idiom. Any other asm gets
nondet register outputs from the walker, or UNKNOWN when reached if it has a
memory output or a "memory" clobber: giving that memory nondet contents is
what #111 builds for bodyless calls, not yet on main, and a "memory" clobber
would need all of memory, globals included.

LDV tasks, 60 s each, before (main) and after, of the 844 tasks main left
UNKNOWN on an asm or on llvm.bswap, or timed out:

| before | tasks | after |
|---|---|---|
| UNKNOWN, asm | 401 | 302 UNKNOWN at a variadic function, 32 UNKNOWN at an asm that may write memory, 10 UNKNOWN at an asm with two outputs, one a pointer (the encoder has no struct of a pointer), 10 UNKNOWN at other constructs, 44 timeouts, 1 killed at 57 s, 2 FAILED (expected true) |
| UNKNOWN, llvm.bswap.i16 | 8 | 7 UNKNOWN at llvm.prefetch, 1 timeout |
| timeout | 435 | 434 timeouts, 1 UNKNOWN at a variadic function |

No verdict became right. Few verdicts change because most of these tasks stop next at a variadic
function the task defines (printk, dev_err), which the walker does not
support: 1,463 of the 2,358 LDV unreach-call tasks give that UNKNOWN on main.
The wrong FAILED results come from `current` (`get_current()`, a per-cpu
read): its value is a nondet pointer, which may point to the driver's own
objects, so `set_current_state()`, an xchg through it, may write them. Main
gives such results already for pointers from bodyless calls (20 LDV tasks
expected true are FAILED on main).
