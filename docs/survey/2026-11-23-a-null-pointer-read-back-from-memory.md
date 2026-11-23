# A null pointer read back from memory

Recorded 2026-11-23.

The first ReachSafety baseline gave wrong FALSE verdicts on seven heap-data
tasks, ldv-regression/rule60_list2 and simple_and_skiplist_2lvl-2 (issues
#6, #7, #8). All build a list in one loop and walk it in another, and all
come down to this program, which Vereda called unsafe:

```c
struct node *l = 0;
if (nondet()) {
  struct node *p = malloc(sizeof(*p));
  if (!p) abort();
  p->a = 0; p->next = 0; l = p;
}
if (l) assert(l->next == 0);
```

A store on one path leaves bytes that the known-byte map no longer holds
after the join, so the read goes to the data and provenance arrays and
rebuilds the pointer as (provenance, data - bases[provenance]). For the
stored null the provenance is object 0, and bases[0] was free: the
constructor recorded the null object before constraining its base, so
base(0) folded to the recorded constant and the constraint to true. The
null read back as object 0 at an arbitrary offset, which is not null, and
the walk went on through it. Without the join the known bytes answer the
read and the bug stays hidden, which is why the suite missed it.

The fix constrains bases[0] to 0 before recording the object, as
`allocate` already does. rule60_list2 now proves TRUE; the other eight
tasks run out of the 60 s limit instead of answering FALSE.
