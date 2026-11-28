// A 32-byte vector through ... on i386 goes on the stack aligned to 32,
// more than the stack's 16 at a call, which the walker does not place:
// unsupported, not a guess.
// RUN: -m32
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: variadic call with argument <8 x float>
#include <stdarg.h>
extern void __VERIFIER_assert(int);

typedef float v8f __attribute__((vector_size(32)));

float first(int n, ...) {
  va_list ap;
  va_start(ap, n);
  v8f v = va_arg(ap, v8f);
  va_end(ap);
  return v[0];
}

int main(void) {
  v8f v = {1.0f};
  __VERIFIER_assert(first(0, v) == 1.0f);
  return 0;
}
