#include <assert.h>

int main() {
  _ExtInt(10) x = __VERIFIER_nondet_float();
  _ExtInt(10) y = __VERIFIER_nondet_int();
  _ExtInt(10) z = x + y;
  assert(z == x + y);
}
// Imported from ESBMC regression/esbmc/bitvector_04
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
