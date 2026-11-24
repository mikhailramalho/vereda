# Inline asm atomics through a pointer to an unknown object

Recorded 2026-11-24.

`2026-11-24-inline-asm-after-variadics.md` found 11 LDV tasks expected true
that #125 turned from UNKNOWN into FAILED, 9 of them through a kernel atomic
or bit operation that writes through a pointer whose object is not known:
`current` (a per-cpu read, so a nondet pointer) or a field of a driver struct
the environment model never filled. Such a pointer may point to any object,
the LDV model's lock counters included.

`ir` now marks the writes it makes of these asm (the atomicrmw, cmpxchg and
the store of a sete/setc flag) with `vereda.asm` metadata, and the walker
gives UNKNOWN, naming the asm, when such a write goes through a pointer whose
object is neither one constant object nor one of two (a pointer from an
allocation that may fail). A pointer to a known object keeps the exact
atomic. Giving a pointer that comes from outside the program an object of
its own would keep these tasks going; it is not built here.

LDV unreach-call tasks, 60 s each, main (5d657abd) against this change:

| tasks | main | after |
|---|---|---|
| the 11 wrong FAILED | FAILED | 9 UNKNOWN at the asm (xchg, bts, btr), 2 FAILED |
| the 5 #125 made right | 3 SUCCESSFUL, 2 FAILED | the same |
| the 95 main decides | 51 FAILED (expected true), 25 SUCCESSFUL, 19 FAILED (expected false) | unchanged |
| 120 others, picked at random | 97 timeouts, 5 killed shortly before 60 s, 18 UNKNOWN | 22 of the timeouts and 2 of the killed ones now UNKNOWN at an asm atomic through an unknown object, the rest unchanged |

The 2 still FAILED do not come from an atomic. Both run an MMIO write
(`writel`, which #125 turns into a volatile store) through a pointer from the
model's `ioremap` (`ldv_malloc(0)`), and both also write through pointers
built by integer arithmetic on addresses (CIL's `__cil_tmp` casts), which
may land in any object: in altera_jtaguart a store to
`altera_jtaguart_ports[i]`, `i` read from an unfilled `pdev->id`. Refusing
the MMIO write the same way, tried apart from this change, gives UNKNOWN on
both; it is not done here, as a C store through such a pointer writes
anywhere on main too.
