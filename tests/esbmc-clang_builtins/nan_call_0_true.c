#include <math.h>

int main() {
  double f1 = __builtin_nan("12");
  if(!isnan(f1)) __VERIFIER_error();
  return 0;
}
// Imported from ESBMC regression/clang_builtins/nan_call_0_true
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
