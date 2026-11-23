/* github #8700: int and _BitInt are not promoted, so ++ on them can overflow */
#include <limits.h>
int main(void)
{
  signed char c = 127;
  c++;
  int i = INT_MAX;
  i++;
  _BitInt(7) b = 63;
  ++b;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8700_fail
// RUN: --overflow-check --multi-property
// EXPECT: ^  PASSED .*line +6  arithmetic overflow on add$
// EXPECT: ^  FAILED .*line +8  arithmetic overflow on add$
// EXPECT: ^  FAILED .*line +10  arithmetic overflow on add$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  PASSED .*line +6  arithmetic overflow on add$
// MODE: FUTURE
