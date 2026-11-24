// A variadic function reads back what its caller passed: ints, longs, long
// longs, doubles, a pointer that still points to its object, structs clang
// splits into registers or passes byval, and more arguments than x86-64 has
// registers for, so that the rest go on the stack; va_copy gives a second
// cursor that starts where the first one was. variadic_args_m32.c runs this
// for i386.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdarg.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

struct pair { long a; double b; };
struct big { long a, b, c; };
struct tiny { char c; };

int x;

void check(int n, ...) {
  va_list ap, aq;
  va_start(ap, n);
  int i = va_arg(ap, int);
  long l = va_arg(ap, long);
  long long ll = va_arg(ap, long long);
  double d = va_arg(ap, double);
  int *p = va_arg(ap, int *);
  struct pair s = va_arg(ap, struct pair);
  struct big b = va_arg(ap, struct big);
  va_copy(aq, ap);
  struct tiny t = va_arg(ap, struct tiny);
  long sum = 0;
  for (int k = 0; k < 6; k++)
    sum += va_arg(ap, long);
  double dsum = 0;
  for (int k = 0; k < 9; k++)
    dsum += va_arg(ap, double);
  va_end(ap);
  struct tiny again = va_arg(aq, struct tiny);
  va_end(aq);
  __VERIFIER_assert(i == n && l == -2000000000L && ll == -5000000000LL && d == 0.5);
  __VERIFIER_assert(p == &x && *p == n);
  __VERIFIER_assert(s.a == n + 1 && s.b == 2.5 && b.a == 1 && b.c == n);
  __VERIFIER_assert(t.c == 'v' && again.c == 'v' && sum == 21 && dsum == 45.0);
}

int main(void) {
  int n = __VERIFIER_nondet_int();
  x = n;
  struct pair s = {n + 1, 2.5};
  struct big b = {1, 2, n};
  struct tiny t = {'v'};
  check(n, n, -2000000000L, -5000000000LL, 0.5, &x, s, b, t, 1L, 2L, 3L, 4L, 5L, 6L,
        1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
  return 0;
}
