// variadic_long_double.c's call, but the second long double read is taken
// for the first: wrong unless the two are equal.
// RUN: -m64
// RUN: -m32
// EXPECT: ^VERIFICATION FAILED$
#include <stdarg.h>
extern long double __VERIFIER_nondet_longdouble(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static long double q;

static void take(int n, ...) {
  va_list ap;
  va_start(ap, n);
  for (int i = 0; i < n; ++i)
    (void)va_arg(ap, int);
  (void)va_arg(ap, int);
  (void)va_arg(ap, long double);
  (void)va_arg(ap, double);
  q = va_arg(ap, long double);
  va_end(ap);
}

int main(void) {
  long double x = __VERIFIER_nondet_longdouble(), y = __VERIFIER_nondet_longdouble();
  __VERIFIER_assume(x == x && y == y);
  take(5, 1, 2, 3, 4, 5, __VERIFIER_nondet_int(), x, 2.5, y, 7);
  __VERIFIER_assert(q == x);
  return 0;
}
