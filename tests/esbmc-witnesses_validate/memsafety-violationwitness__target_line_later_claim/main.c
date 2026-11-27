#include <stdlib.h>

int g(void)
{
  return 1;
}

int main()
{
  char *p = malloc(10);
  char *q = malloc(10);
  if (!p || !q)
    return 0;
  free(p);
  if (q[0] == 'a' && g() && p[2] == 'a')
    return 1;
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/target_line_later_claim
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --validate-violation-witness --witness witness.yml --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\s+FAILED\s+\[main\.invalidated-dynamic-object\.\d+\]\s+line 15
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
