#include <assert.h>

int main() {
  int x = __VERIFIER_nondet_int();
  assert((x ^ x) != 0);
  return 0;
}

// Imported from ESBMC regression/esbmc/simplifier3_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
