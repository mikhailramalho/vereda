// A kernel bit operation through a pointer to a known object works, here one
// malloc gives, which may also be null, at a nondet bit.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

struct dev { unsigned long flags; int locked; };

static void set_bit(long nr, volatile unsigned long *addr) {
  __asm__ __volatile__("lock; btsq %1,%0" : "+m"(*(volatile long *)addr) : "Ir"(nr) : "memory");
}

int main(void) {
  struct dev *d = malloc(sizeof *d);
  if (!d)
    return 0;
  d->flags = 0;
  d->locked = 0;
  int nr = __VERIFIER_nondet_int() & 63;
  set_bit(nr, &d->flags);
  __VERIFIER_assert(d->flags == 1UL << nr && d->locked == 0);
  return 0;
}
