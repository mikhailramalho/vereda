#include <assert.h>

unsigned char _x, _y, sum;

void func_overflow() {
  _x = 105;
  _y = 240;
  sum = _x + _y;
  assert(sum > 100); // user defined property
}

int main() {
  return 0;
}
// Imported from ESBMC regression/esbmc-old/github_597_oldfrontend_2
// RUN: --entry func_overflow --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
