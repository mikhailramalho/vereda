#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

int depth(int n)
{
  if (n > 0)
    return depth(n - 1);
  return 0;
}

int main()
{
  int *p = malloc(sizeof(int));
  depth(__VERIFIER_nondet_int());
  p = 0;
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/recursion_before_target
// Needs: --memory-leak-check --validate-violation-witness --witness witness.yml
// RUN: --memory-leak-check --incremental-bmc --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^\s+FAILED\s+\[main\.memory-leak\.\d+\]\s+line 18
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
