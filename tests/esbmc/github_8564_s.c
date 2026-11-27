#include <assert.h>
#include <stdio.h>
int main(void)
{
  char name[11];
  for (int i = 0; i < 10; i++)
    name[i] = 'A';
  name[10] = '\0';
  int r = printf("%s", name);
  assert(r == 10);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8564_s
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
