// A variadic function walks as many arguments as its first one says, a
// nondet count; variadic_count_m32.c runs this for i386.
// RUN: -m64 --unwind 6
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdarg.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern int __VERIFIER_nondet_int(void);

int sum(int n, ...) {
  va_list ap;
  va_start(ap, n);
  int s = 0;
  for (int i = 0; i < n; i++)
    s += va_arg(ap, int);
  va_end(ap);
  return s;
}

int main(void) {
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n >= 0 && n <= 5);
  __VERIFIER_assert(sum(n, 1, 2, 3, 4, 5) == n * (n + 1) / 2);
  __VERIFIER_assert(sum(0) == 0);
  return 0;
}
