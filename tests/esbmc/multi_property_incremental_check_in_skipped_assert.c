/* Every claim raised in the closing round's base case is promoted when the
   forward condition holds. */
#include <assert.h>

int main()
{
  int a[3] = {0, 0, 0};
  int *p = a;
  for (int i = 0; i < 3; i++)
  {
    assert(*p == 0);
    p++;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_incremental_check_in_skipped_assert
// RUN: --incremental-bmc --multi-property --max-k-step 6
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^\s+PASSED\s+\[main\.array-bounds-violated\.1\]\s+line\s+11\s+dereference failure: array bounds violated$
// Vereda: unmet EXPECT: ^\s+PASSED\s+\[main\.array-bounds-violated\.1\]\s+line\s+11\s+dereference failure: array bounds violated$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
