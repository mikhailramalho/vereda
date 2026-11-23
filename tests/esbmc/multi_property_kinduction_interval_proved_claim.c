/* The twin of multi_property_kinduction_interval_missed_claim with a loop
   assertion that holds: its PASSED row must survive, and assert(1==2) must
   still be reported (esbmc/esbmc#7900). */
#include <assert.h>
int main()
{
  for (int i = 0; i < 2; i++)
  {
    assert(i < 5);
  }
  assert(1 == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_kinduction_interval_proved_claim
// Needs: --k-induction --interval-analysis
// RUN: --k-induction --interval-analysis --multi-property --max-k-step 10
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
