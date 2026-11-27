#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

DEFINE_ESBMC_OVERFLOW_TYPE(char)

int main() {
  char a = __VERIFIER_nondet_char();
  char b = __VERIFIER_nondet_char();

  __ESBMC_overflow_result foo;

  foo = __ESBMC_overflow_result_plus(a, b);
  
  if (a == 100 && b == 100)
    assert(!foo.overflow);

  return 0;
}


// Imported from ESBMC regression/esbmc/overflow-result3
// Needs: __ESBMC_overflow_result_plus
// Needs: REQUIRES signed_char_host
// RUN: 
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
