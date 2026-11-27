#include <assert.h>

int main() {
  int a[4];
  char *p = (char *)&a[2];
  assert(__ESBMC_r_ok(p, sizeof(int) * 3));
}
// Imported from ESBMC regression/esbmc/r_ok22
// Needs: __ESBMC_r_ok
// RUN: --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
