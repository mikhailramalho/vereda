// A kernel atomic through a pointer from outside the program, here
// `current`, a per-cpu read that gives a nondet pointer, cannot reach the
// lock counter below, whose address the program never gives out (#137).
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
