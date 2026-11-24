#include <assert.h>

int main()
{
  int i = 0;
  int sum = 0;

  __ESBMC_loop_invariant(i >= 0 && i <= 5000000 && sum == i * 10);
  int j =
    0; //should pass? should fail? Currently it is not supported and would cause continue unwinding
  while (i < 5000000)
  {
    sum += 10;
    i++;
  }
  assert(sum == 50000000);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/7-not_conjunct_invariant
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
