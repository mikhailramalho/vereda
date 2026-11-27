#include <assert.h>

int main() {
  int x,y;
  // must fail
  assert((x-y>0) == (x>y));
}
// Imported from ESBMC regression/cbmc/01_cbmc_BV_Arithmetic1
// EXPECT: ^VERIFICATION FAILED$
