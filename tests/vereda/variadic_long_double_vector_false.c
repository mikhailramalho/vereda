// As variadic_long_double_vector: the second element is 2.5, so the
// assertion fails only for n == 1 (#491).
// EXPECT: ^VERIFICATION FAILED$
#include <stdarg.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

typedef long double v2ld __attribute__((vector_size(32)));

long double pick(int n, ...) {
  va_list ap;
  va_start(ap, n);
  int a = va_arg(ap, int);
  v2ld v = va_arg(ap, v2ld);
  int b = va_arg(ap, int);
  va_end(ap);
  return v[n] + a * 100 + b * 1000;
}

int main(void) {
  int n = __VERIFIER_nondet_int();
  if (n < 0 || n > 1)
    return 0;
  v2ld v = {1.5L, 2.5L};
  __VERIFIER_assert(pick(n, 7, v, 9) == 9701.5L);
  return 0;
}
