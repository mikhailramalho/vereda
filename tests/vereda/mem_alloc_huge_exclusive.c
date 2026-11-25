// Mutually exclusive constant-size allocations that together exceed the
// address space must remain feasible on their respective branches:
// unconditional layout reservation must not apply past the space budget.
// RUN: -m32 --force-malloc-success
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_error(void);

int main(void) {
  char *p;
  if (__VERIFIER_nondet_int())
    p = malloc(1500000000U);
  else
    p = malloc(1500000000U);
  if (__VERIFIER_nondet_int()) {
    char *q = malloc(1500000000U);
    (void)q;
  }
  if (p)
    __VERIFIER_error();
  return 0;
}
