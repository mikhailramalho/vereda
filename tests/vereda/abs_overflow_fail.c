// abs and labs of the least value overflow and are reported as an
// overflow on abs; a negation of the least value stays an overflow on neg.
// RUN: --ub-signed-overflow-check --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main\.arithmetic-overflow\.1\]\s+line 15\s+arithmetic overflow on abs
// EXPECT: FAILED\s+\[main\.arithmetic-overflow\.2\]\s+line 16\s+arithmetic overflow on abs
// EXPECT: FAILED\s+\[main\.arithmetic-overflow\.3\]\s+line 17\s+arithmetic overflow on neg
#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);
extern long __VERIFIER_nondet_long(void);

int main(void) {
  int x = __VERIFIER_nondet_int(), z = __VERIFIER_nondet_int();
  int a = abs(x);
  long b = labs(__VERIFIER_nondet_long());
  int c = -z;
  return a + (int)b + c;
}
