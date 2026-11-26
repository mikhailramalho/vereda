// long double (x86_fp80) classification (isnan / llvm.is.fpclass).
// RUN: -m64
// RUN: -m64 --fp2bv
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>
extern void __VERIFIER_assert(int);
static long double g;
int main(void) {
  __VERIFIER_assert(!isnan(g));
  return 0;
}
