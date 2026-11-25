/* Two claims violated at different k. Both belong in one table, and the
   forward condition's unwinding assertion is not a property of the program. */
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
// Imported from ESBMC regression/esbmc/multi_property_incremental_one_table
// RUN: --incremental-bmc --multi-property --max-k-step 10
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Properties: 2 checked, \d+ passed, 2 failed, \d+ not checked$
// EXPECT: \A(?![\s\S]*VERIFICATION SUCCESSFUL)[\s\S]*\Z
