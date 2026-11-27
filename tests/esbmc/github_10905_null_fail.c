#include <assert.h>
#include <stdint.h>
int main(void)
{
  char *p = (char *)(uintptr_t)5;
  char *q = p - 5;
  assert(q == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10905_null_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
