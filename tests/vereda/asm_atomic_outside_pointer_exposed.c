// A kernel atomic through a pointer from outside the program, here
// `current`, may reach the lock counter once the program gave its address
// out (#137).
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern void register_lock(long *);

long locked;

static long *get_current(void) {
  long *t;
  __asm__("movq %%gs:0,%0" : "=r"(t));
  return t;
}

static long xchg(long *p, long v) {
  __asm__ __volatile__("xchgq %0,%1" : "+r"(v), "+m"(*p) : : "memory");
  return v;
}

int main(void) {
  register_lock(&locked);
  xchg(get_current(), 1);
  __VERIFIER_assert(locked == 0);
  return 0;
}
