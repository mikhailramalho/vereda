// RUN: --unwind 8 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Conditional memmove on a summarised integer array inside a loop exercises
// copyArray lambdas, Terms::constArray memoisation, and DAG lambda tracking
// without generating copy_provenance lambdas or cubic under-table copies.
#include <string.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char x[256];
  for (int i = 0; i < 256; ++i)
    x[i] = (char)(i & 127);
  int shifts = 0;
  for (int k = 0; k < 6; ++k) {
    if (__VERIFIER_nondet_int()) {
      memmove(&x[0], &x[1], 255);
      ++shifts;
    }
  }
  __VERIFIER_assert(x[0] == shifts);
  __VERIFIER_assert(x[1] == shifts + 1);
  return 0;
}
