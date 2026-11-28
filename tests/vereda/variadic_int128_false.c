// variadic_int128.c's second call, but y is taken for x: wrong unless the
// two are equal.
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
#include <stdarg.h>
extern __int128 __VERIFIER_nondet_int128(void);
extern void __VERIFIER_assert(int);

static __int128 y2;

static void take(int n, ...) {
  va_list ap;
  va_start(ap, n);
  for (int i = 0; i < n; ++i)
    (void)va_arg(ap, int);
  (void)va_arg(ap, __int128);
  (void)va_arg(ap, int);
  (void)va_arg(ap, int);
  y2 = va_arg(ap, __int128);
  va_end(ap);
}

int main(void) {
  __int128 x = __VERIFIER_nondet_int128(), y = __VERIFIER_nondet_int128();
  take(4, 1, 2, 3, 4, x, 5, 7, y, 9);
  __VERIFIER_assert(y2 == x);
  return 0;
}
