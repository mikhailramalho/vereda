#include <assert.h>
#include <stdio.h>

int main(void)
{
  char b[100];
  char name[11] = "AAAAAAAAAA";
  char *p = __VERIFIER_nondet_int() ? name : 0;

  int r = snprintf(b, sizeof(b), "%s", p);
  assert(r == 10);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8524_null_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*FAILED\s+\[main\.null-pointer-dereference\.\d+\]\s+line 10\s+dereference failure: NULL pointer$
// Vereda: unmet EXPECT: ^\s*FAILED\s+\[main\.null-pointer-dereference\.\d+\]\s+line 10\s+dereference failure: NULL pointer$
// MODE: FUTURE
// Vereda: verdict FAILED
