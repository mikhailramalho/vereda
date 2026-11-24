// A kernel atomic through a pointer whose object is not known, here
// `current`, a per-cpu read that may give any pointer, is unsupported: the
// write could reach any object, the lock counter below included.
// RUN: -m64
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: inline asm `xchg \$0,\$1` through a pointer to an unknown object at
extern void __VERIFIER_assert(int);

struct task { long state; };
int locked;

static struct task *get_current(void) {
  struct task *t;
  __asm__("movq %%gs:0,%0" : "=r"(t));
  return t;
}

static long xchg(long *p, long v) {
  __asm__ __volatile__("xchgq %0,%1" : "+r"(v), "+m"(*p) : : "memory");
  return v;
}

int main(void) {
  xchg(&get_current()->state, 1);
  __VERIFIER_assert(locked == 0);
  return 0;
}
