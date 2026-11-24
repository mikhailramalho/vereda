#include <assert.h>
#include <stdio.h>

int main(void)
{
  char b[1200];
  char name[11];

  for (int i = 0; i < 10; i++)
    name[i] = 'A';
  name[5] = __VERIFIER_nondet_char();
  name[10] = '\0';

  int r = snprintf(b, sizeof(b), "%s", name);
  assert(r == 10);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8524_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s+r = 5; //
// Vereda: unmet EXPECT: ^\s+r = 5 \(
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
