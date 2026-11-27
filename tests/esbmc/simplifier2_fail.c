#include <assert.h>

int main() {
  unsigned u = __VERIFIER_nondet_uint();
  assert(u < 0);
  return 0;
}

// Imported from ESBMC regression/esbmc/simplifier2_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
