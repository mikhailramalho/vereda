#include <assert.h>
#include <limits.h>
#include <stdlib.h>

int __VERIFIER_nondet_int(void);
long __VERIFIER_nondet_long(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  long y = __VERIFIER_nondet_long();
  __VERIFIER_assume(x > INT_MIN && y > LONG_MIN);
  assert(abs(x) >= 0);
  assert(labs(y) >= 0);
  assert(__builtin_abs(x) == abs(x));
  return 0;
}
// Imported from ESBMC regression/esbmc/abs_overflow_check
// RUN: --overflow-check --multi-property
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^\*\* 0 of 10 properties failed, 10 passed$
// EXPECT: PASSED\s+\[main.arithmetic-overflow.7\]\s+line 15\s+arithmetic overflow on abs
// Vereda: unmet EXPECT: ^\*\* 0 of 10 properties failed, 10 passed$
// MODE: FUTURE
