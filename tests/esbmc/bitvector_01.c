#include <assert.h>

int main() {
  _ExtInt(10) x = __VERIFIER_nondet_float();
  _ExtInt(10) y = __VERIFIER_nondet_int();
  assert(x == y);
}
// Imported from ESBMC regression/esbmc/bitvector_01
// EXPECT: ^VERIFICATION FAILED$
