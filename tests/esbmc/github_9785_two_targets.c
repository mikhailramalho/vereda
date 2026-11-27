/* #9785: a pointer made from an integer that may point into either array
 * takes its offset from the integer within each; checked with pointer checks
 * on. */
#include <stdint.h>
#include <assert.h>
_Bool __VERIFIER_nondet_bool(void); unsigned __VERIFIER_nondet_uint(void);
int a[4], b[4];
int main(void) {
  _Bool c = __VERIFIER_nondet_bool(); unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 4);
  uintptr_t base = c ? (uintptr_t)a : (uintptr_t)b;
  int *p = (int *)(base + 4 * k);
  *p = 7;
  assert(c ? a[k] == 7 : b[k] == 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9785_two_targets
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
