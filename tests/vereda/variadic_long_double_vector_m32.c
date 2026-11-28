// On i386 a vector of two long doubles is passed directly as an LLVM
// <2 x x86_fp80>, whose elements lie 10 bytes apart, while C's type puts
// them 12 apart in 32 bytes: the call and clang's own va_arg code then
// disagree, and a native run of the same program with an int on each side
// of the vector reads wrong values for the vector and the int after it.
// With no layout both sides agree on, the call stays unsupported (#491).
// RUN: -m32
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: variadic call with argument <2 x x86_fp80>
#include <stdarg.h>
extern void __VERIFIER_assert(int);

typedef long double v2ld __attribute__((vector_size(24)));

long double pick(int n, ...) {
  va_list ap;
  va_start(ap, n);
  v2ld v = va_arg(ap, v2ld);
  va_end(ap);
  return v[n];
}

int main(void) {
  v2ld v = {1.5L, 2.5L};
  __VERIFIER_assert(pick(0, v) == 1.5L);
  return 0;
}
