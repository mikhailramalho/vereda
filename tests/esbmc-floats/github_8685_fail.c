/* github #8685: each assertion contradicts C11 7.12.6.6 */
#include <assert.h>
#include <math.h>
int main(void)
{
  double tiny = 0x1p-1074; /* smallest subnormal */
  assert(ldexp(tiny, -1) != 0.0);
  assert(ldexp(tiny, 1) != 0x1p-1073);
  assert(ldexp(1.5, -1074) != 0x1p-1073);
  return 0;
}
// Imported from ESBMC regression/floats/github_8685_fail
// RUN: --multi-property
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.1\]\s+line\s+7\s
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.2\]\s+line\s+8\s
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.3\]\s+line\s+9\s
// EXPECT: ^VERIFICATION FAILED$
