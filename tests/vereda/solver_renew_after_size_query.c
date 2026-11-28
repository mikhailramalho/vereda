// Keeping a symbolic-size query's state for the first solve must not keep
// that phase's asserted violation when checking the unwinding property.
// RUN: --unwind 1
// RUN: --unwind 1 --fresh-solver
// EXPECT: SUCCESS +\[main.assertion.1\]
// EXPECT: FAILED +\[main.assertion.2\].*unwinding assertion loop 0
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 4);
  char src[4] = {7, 7, 7, 7}, dst[4] = {0};
  memcpy(dst, src, n);
  __VERIFIER_assert(dst[0] + dst[1] + dst[2] + dst[3] == 7 * n);
  while (__VERIFIER_nondet_uint()) {}
}
