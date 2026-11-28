// A variadic function reads back __int128 arguments (#305). On x86-64 one
// takes two general registers when two are free, else 16 bytes of the stack
// aligned to 16, and an int after it still takes a register left free.
// With n = 0, x takes two registers and y the stack while b takes the last
// register; with n = 4, x and y go on the stack, y past c's slot.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdarg.h>
extern __int128 __VERIFIER_nondet_int128(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

static int digits, k2, c2, b2;
static __int128 x2, y2;

static void take(int n, ...) {
  va_list ap;
  va_start(ap, n);
  digits = 0;
  for (int i = 0; i < n; ++i)
    digits = digits * 10 + va_arg(ap, int);
  x2 = va_arg(ap, __int128);
  k2 = va_arg(ap, int);
  c2 = va_arg(ap, int);
  y2 = va_arg(ap, __int128);
  b2 = va_arg(ap, int);
  va_end(ap);
}

int main(void) {
  __int128 x = __VERIFIER_nondet_int128(), y = __VERIFIER_nondet_int128();
  int k = __VERIFIER_nondet_int();
  take(0, x, k, 7, y, 9);
  __VERIFIER_assert(digits == 0);
  __VERIFIER_assert(x2 == x && k2 == k && c2 == 7 && y2 == y && b2 == 9);
  take(4, 1, 2, 3, 4, x, k, 7, y, 9);
  __VERIFIER_assert(digits == 1234);
  __VERIFIER_assert(x2 == x && k2 == k && c2 == 7 && y2 == y && b2 == 9);
  return 0;
}
