#include <assert.h>

int main() {
  int a[4];
  char *p = (char *)&a[0];
  assert(__ESBMC_r_ok(p, sizeof(int) * 4));
}
// Imported from ESBMC regression/esbmc/r_ok16
// Needs: __ESBMC_r_ok
// RUN: --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
