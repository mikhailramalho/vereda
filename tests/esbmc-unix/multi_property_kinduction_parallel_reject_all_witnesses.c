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
// Imported from ESBMC regression/esbmc-unix/multi_property_kinduction_parallel_reject_all_witnesses
// Needs: --k-induction-parallel --all-witnesses
// RUN: --k-induction-parallel --all-witnesses --max-k-step 10
// MODE: FUTURE
// EXPECT: ^ERROR: --k-induction-parallel cannot be combined with --all-witnesses$
// Vereda: verdict none
