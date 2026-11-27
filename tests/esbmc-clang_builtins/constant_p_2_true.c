#include <math.h>

int main() {
  if(!__builtin_constant_p(42)) __VERIFIER_error();
  return 0;
} 
// Imported from ESBMC regression/clang_builtins/constant_p_2_true
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
