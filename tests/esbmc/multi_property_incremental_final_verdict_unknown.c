/* The twin of multi_property_incremental_final_verdict: no claim is violated
   within --max-k-step, so exhausting the bound must still report
   VERIFICATION UNKNOWN (esbmc/esbmc#7900). */
#include <assert.h>
int main()
{
  unsigned x = 0;
  while (1)
  {
    assert(x != 100);
    ++x;
  }
}
// Imported from ESBMC regression/esbmc/multi_property_incremental_final_verdict_unknown
// RUN: --incremental-bmc --multi-property --max-k-step 6
// EXPECT: ^Unable to prove or falsify the program, giving up\.$
// EXPECT: ^VERIFICATION UNKNOWN$
// EXPECT: \A(?![\s\S]*VERIFICATION FAILED)[\s\S]*\Z
// Vereda: unmet EXPECT: ^Unable to prove or falsify the program, giving up\.$
// MODE: FUTURE
// Vereda: verdict FAILED
