// x86-64 passes a struct of two floats through ... as a vector in one SSE
// register, which the walker does not place: unsupported, not a guess.
// RUN: -m64
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: variadic call with argument <2 x float>
#include <stdarg.h>
extern void __VERIFIER_assert(int);

struct floats { float a, b; };

float first(int n, ...) {
  va_list ap;
  va_start(ap, n);
  struct floats f = va_arg(ap, struct floats);
  va_end(ap);
  return f.a;
}

int main(void) {
  struct floats f = {1.0f, 2.0f};
  __VERIFIER_assert(first(0, f) == 1.0f);
  return 0;
}
