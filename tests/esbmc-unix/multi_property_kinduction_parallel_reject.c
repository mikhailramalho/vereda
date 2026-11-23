/* esbmc/esbmc#7900: the parallel driver cannot merge per-claim verdicts. */
#include <assert.h>

int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  unsigned x = 0;
  assert(n != 7);
  for (unsigned i = 0; i < n; ++i)
    ++x;
  assert(x != 3);
}
// Imported from ESBMC regression/esbmc-unix/multi_property_kinduction_parallel_reject
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel --multi-property --max-k-step 10
// MODE: FUTURE
// EXPECT: ^ERROR: --k-induction-parallel cannot be combined with --multi-property$
// Vereda: verdict none
