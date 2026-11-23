/* github #8682: each assertion contradicts C11 F.10.1.4 */
#include <assert.h>
#include <math.h>
int main(void)
{
  assert(atan2(-0.0, -1.0) > 0.0);
  assert(atan2(0.0, -0.0) == 0.0);
  assert(!signbit(atan2(-0.0, 1.0)));
  assert(!isnan(atan2(NAN, 0.0)));
  assert(isnan(atan2(INFINITY, INFINITY)));
  return 0;
}
// Imported from ESBMC regression/floats/github_8682_fail
// RUN: --multi-property
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.1\]\s+line\s+6\s
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.2\]\s+line\s+7\s
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.3\]\s+line\s+8\s
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.4\]\s+line\s+9\s
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.5\]\s+line\s+10\s
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s+FAILED\s+\[main\.assertion\.2\]\s+line\s+7\s
// MODE: FUTURE
// Vereda: verdict FAILED
