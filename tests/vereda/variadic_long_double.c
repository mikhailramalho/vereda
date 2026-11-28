// A variadic function reads back long doubles among other arguments (#305).
// On x86-64 they go on the stack, 16 bytes aligned to 16, here after an int
// that the full general registers left there too; on i386 they take 12
// bytes of the stack.
// RUN: -m64
// RUN: -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdarg.h>
extern long double __VERIFIER_nondet_longdouble(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static int digits, a, b;
static double d;
static long double p, q;

static void take(int n, ...) {
  va_list ap;
  va_start(ap, n);
  for (int i = 0; i < n; ++i)
    digits = digits * 10 + va_arg(ap, int);
  a = va_arg(ap, int);
  p = va_arg(ap, long double);
  d = va_arg(ap, double);
  q = va_arg(ap, long double);
  b = va_arg(ap, int);
  va_end(ap);
}

int main(void) {
  long double x = __VERIFIER_nondet_longdouble(), y = __VERIFIER_nondet_longdouble();
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(x == x && y == y);
  take(5, 1, 2, 3, 4, 5, k, x, 2.5, y, 7);
  __VERIFIER_assert(digits == 12345);
  __VERIFIER_assert(a == k);
  __VERIFIER_assert(p == x);
  __VERIFIER_assert(d == 2.5);
  __VERIFIER_assert(q == y);
  __VERIFIER_assert(b == 7);
  return 0;
}
