/* Negative counterpart of github_7597-array: the element written the symbolic
 * value must keep it, not the counter's. */
#include <assert.h>

int A[3];

int __VERIFIER_nondet_int(void);

int main(void)
{
  int in = __VERIFIER_nondet_int();

  for (A[0] = 1; A[0] <= 5; A[0] = A[0] + 1)
    A[1] = in;

  assert(A[1] == 6);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7597-array_fail
// Needs: --no-standard-checks
// RUN: --no-standard-checks --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
