#include <assert.h>

int main()
{
  int i = 0;
  int sum = 0;
  while (i < 5000000)
  {
    __ESBMC_loop_invariant(i >= 0 && i <= 5000000 && sum == i * 10);
    sum += 10;
    i++;
  }
  assert(sum == 50000000);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/6-invariant_in_wrong_place
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property --unwind 1 --no-unwinding-assertions
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
