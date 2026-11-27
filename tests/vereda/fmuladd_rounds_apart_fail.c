// Under clang's default -ffp-contract=on a * a + c became llvm.fmuladd,
// which Vereda and InstCombine's folding fused, so the assertion held; x86-64
// without FMA rounds the product and the sum apart, and the built program
// fails it. vereda.py now turns contraction off.
// EXPECT: ^VERIFICATION FAILED$
#include <assert.h>
#include <math.h>
int main(void) {
  double a = 0x1.fffffffffffffp0;
  double c = -0x1.fffffffffffffp1;
  assert(fma(a, a, c) == a * a + c);
  return 0;
}
