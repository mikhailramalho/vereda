/* github #8683: each assertion contradicts C11 7.12.7.3 or F.10.4.3 */
#include <assert.h>
#include <math.h>
int main(void)
{
  assert(isinf(hypot(1e200, 1e200)));
  assert(hypot(1e-200, 1e-200) == 0.0);
  assert(isnan(hypot(INFINITY, NAN)));
  return 0;
}
// Imported from ESBMC regression/floats/github_8683_fail
// RUN: --multi-property
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.1\]\s+line\s+6\s
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.2\]\s+line\s+7\s
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.3\]\s+line\s+8\s
// EXPECT: ^VERIFICATION FAILED$
