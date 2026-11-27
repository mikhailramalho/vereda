#include <assert.h>

int main() {
  int x = __VERIFIER_nondet_int();
  assert((x | ~x) != -1);
  return 0;
}

// Imported from ESBMC regression/esbmc/simplifier4_fail
// EXPECT: ^VERIFICATION FAILED$
