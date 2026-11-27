/* github #7119: an __ESBMC_exists assertion prints as written. */
#include <assert.h>

int i;

int main(void)
{
  assert(__ESBMC_exists(&i, i > 0 && i < 0));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7119_exists_fail
// Needs: __ESBMC_exists
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  __ESBMC_exists\(\(void \*\)\(&i\), i > 0 && i < 0\)$
