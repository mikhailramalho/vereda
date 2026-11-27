#include <assert.h>
#include <stdint.h>
int main(void)
{
  char *p = (char *)(uintptr_t)5;
  char *q = p - 5;
  assert(q != 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10905_null
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
