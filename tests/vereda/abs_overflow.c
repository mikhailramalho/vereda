// abs of any value but the least does not overflow, so the check on abs
// passes; without the assumption it fails (abs_overflow_fail.c).
// RUN: --ub-signed-overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <limits.h>
#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x != INT_MIN);
  __VERIFIER_assert(abs(x) >= 0);
  return 0;
}
