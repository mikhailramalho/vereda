// A 32-byte vector through ... on i386 goes on the stack aligned to 32,
// between two ints, which clang's va_arg code finds only if the stack area
// is aligned to 32 too.
// RUN: -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdarg.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

typedef float v8f __attribute__((vector_size(32)));

float pick(int n, ...) {
  va_list ap;
  va_start(ap, n);
  int a = va_arg(ap, int);
  v8f v = va_arg(ap, v8f);
  int b = va_arg(ap, int);
  va_end(ap);
  return v[n] + a * 100 + b * 1000;
}

int main(void) {
  int n = __VERIFIER_nondet_int();
  if (n < 0 || n > 7)
    return 0;
  v8f v = {1, 2, 3, 4, 5, 6, 7, 8};
  __VERIFIER_assert(pick(n, 7, v, 9) == 9701 + n);
  return 0;
}
