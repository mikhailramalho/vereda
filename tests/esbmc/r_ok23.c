#include <assert.h>

int main() {
  int a[4];
  char *p = (char *)&a[2];
  assert(__ESBMC_r_ok(p, sizeof(int) * 2));
}
// Imported from ESBMC regression/esbmc/r_ok23
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
