#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

DEFINE_ESBMC_OVERFLOW_TYPE(short)

int main() {
  short a = __VERIFIER_nondet_short();
  short b = __VERIFIER_nondet_short();

  __VERIFIER_assume(a >= -1000 && a <= 1000);
  __VERIFIER_assume(b >= -1000 && b <= 1000);

  __ESBMC_overflow_result foo;

  foo = __ESBMC_overflow_result_plus(a, b);
  
  if (!foo.overflow)
    assert(foo.result == a + b);

  return 0;
}


// Imported from ESBMC regression/esbmc/overflow-result2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
