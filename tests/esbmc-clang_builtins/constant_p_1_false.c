#include <math.h>

int a = 42;

int main() {
  if(!__builtin_constant_p(a)) __VERIFIER_error();
  return 0;
} 
// Imported from ESBMC regression/clang_builtins/constant_p_1_false
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
