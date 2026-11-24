#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
  char b[1200];
  char name[11];

  for (int i = 0; i < 10; i++)
    name[i] = __VERIFIER_nondet_char();
  name[10] = '\0';

  unsigned char key_len = snprintf(b, sizeof(b), "%s=%d", name, 7);
  assert(key_len == strlen(name) + 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8524_nondet
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
