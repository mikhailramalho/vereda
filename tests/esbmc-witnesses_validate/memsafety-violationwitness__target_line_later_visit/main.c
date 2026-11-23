#include <stdlib.h>

int main()
{
  char *p = malloc(10);
  if (!p)
    return 0;
  int s = 0;
  for (int i = 0; i <= 10; i++) s += p[i];
  return s;
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/target_line_later_visit
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^\s+FAILED\s+\[main\.heap-array-bounds-violated\.\d+\]\s+line 9
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
