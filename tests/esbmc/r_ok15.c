#include <assert.h>

int main() {
  int a[4];
  char *p = (char *)&a[0];
  assert(__ESBMC_r_ok(p, sizeof(int) * 3));
}
// Imported from ESBMC regression/esbmc/r_ok15
// Needs: __ESBMC_r_ok
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
