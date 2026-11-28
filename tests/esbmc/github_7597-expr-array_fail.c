/* Same defect through an array: the counter and the symbolically written
 * element are elements of the *same* object, so one refused element write
 * drops the array and the counter with it. #7605 carried a bare read here
 * too; an operator around it was still refused. */
#include <assert.h>

int A[2];

int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();

  for (A[0] = 1; A[0] <= 5; A[0] = A[0] + 1)
    A[1] = x + 1;

  assert(A[0] == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7597-expr-array_fail
// RUN: --ub-pointer-check=false --ub-div-by-zero-check=false --vla-size-check=false
// EXPECT: ^VERIFICATION FAILED$
