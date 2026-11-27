/* --loop-invariant copies the loop body, so assert(n != 7) is two claims.
   --multi-fail-fast skips the second copy at k = 1 once the first, which
   runs after the havoc, is decided; the base case at k = 2 has to solve it
   rather than drop it with its twin. */
#include <assert.h>

unsigned __VERIFIER_nondet_uint();

int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  unsigned i = 0;
  __ESBMC_loop_invariant(i <= 10);
  while (i < 10)
  {
    assert(n != 7);
    assert(i < 10);
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_loop_invariant_copy_fail
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant --multi-fail-fast 1
// RUN: --loop-invariant --multi-property --multi-fail-fast 1 --max-k-step 2 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
