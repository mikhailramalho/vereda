/* #10918: no object crosses 2^63, so the addresses inside one keep their
 * order and difference as intptr_t. */
#include <assert.h>
#include <stdint.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  uint64_t g[4];
  int i = __VERIFIER_nondet_int(), j = __VERIFIER_nondet_int();
  __VERIFIER_assume(0 <= i && i < 4 && 0 <= j && j < 4);
  intptr_t a = (intptr_t)&g[i], b = (intptr_t)&g[j];
  intptr_t r;
  assert((intptr_t)&g[0] <= (intptr_t)&g[3]);
  assert(!__builtin_sub_overflow(a, b, &r) && r == (i - j) * 8);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10918
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
