/* The entry jump reaches the back edge only through a failed assertion, so
   it got no havoc and the inductive step "proved" i < 500 (esbmc/esbmc#7900). */
#include <assert.h>
int main()
{
  int i = 0;
  goto L;
  while (i < 1000)
  {
    i = i + 2;
  L:
    __VERIFIER_assert(0); // v
    i = i + 1;
  }
  assert(i < 500);
  return 0;
}
// Imported from ESBMC regression/k-induction/multi_property_entry_jump_past_failed_assert
// Needs: --k-induction
// RUN: --k-induction --multi-property --max-k-step 6
// MODE: FUTURE
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.\d+\]\s+line\s+12\s+v$
// EXPECT: \A(?![\s\S]*PASSED\s+\[main\.assertion\.\d+\]\s+line\s+15)[\s\S]*\Z
// Vereda: verdict none
