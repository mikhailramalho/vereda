/* Array counterpart of github_7597: the counter is an element of the same
 * array whose other element is written a symbolic value. */
#include <assert.h>

int A[3];

int __VERIFIER_nondet_int(void);

int main(void)
{
  int in = __VERIFIER_nondet_int();

  for (A[0] = 1; A[0] <= 5; A[0] = A[0] + 1)
    A[1] = in;

  assert(A[0] == 6);
  assert(A[1] == in);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7597-array
// Needs: --no-standard-checks
// RUN: --no-standard-checks
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
