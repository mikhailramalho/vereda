# Stores in counterexamples

Recorded 2026-11-23.

The trace showed only values in registers (debug value records), so a
store into memory was missing: for `a[0] = f(a[0])` with a global `int
a[1]`, the only line was `f`'s parameter `a` receiving its argument,
which read as the array. ESBMC shows `a[0] = 1` and no parameter binding.
Now a source store into a named variable kept in memory (a global, or a
local with a declare record) is a trace step carrying the variable's debug
type and the store's offset term; the witness evaluates the offset in the
model and spells the element or member written (`m[1][2]`, `s.y`, `g[i]`
with the index the model chose). A union member is the first one of the
store's size; a bitfield store, which writes the whole unit, a partial
write and an array of variable length are not shown, rather than named
wrongly. A parameter's record of its own argument is dropped; another
value, another parameter included, stays. Stores through a pointer to a
heap object, which ESBMC spells `p->x`, are not shown yet.

Follow-up issues: #311.
