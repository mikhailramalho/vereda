/* The same claims under plain BMC, where the unwinding bound is the whole
   analysis: there a per-claim UNSAT is a proof and stays one. */
#include <assert.h>

int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  unsigned x = 0;
  __VERIFIER_assume(n < 5);
  for (unsigned i = 0; i < n; ++i)
    ++x;
  assert(x == n);
  assert(x < 5);
}
// Imported from ESBMC regression/esbmc/multi_property_bounded_base_case_row_pass
// RUN: --unwind 10 --multi-property
// EXPECT: ^VERIFICATION SUCCESSFUL$
