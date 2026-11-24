// A pointer passed through ... still points to its object: the callee
// writes through it the sum of a double and a byval struct's field, read
// through a va_copy, and the caller sees the write.
// variadic_pointer_false_m32.c runs this for i386.
// RUN: -m64
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 30  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
#include <stdarg.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

struct big { long a, b, c; };

void store(int n, ...) {
  va_list ap, aq;
  va_start(ap, n);
  int *p = va_arg(ap, int *);
  va_copy(aq, ap);
  va_end(ap);
  double d = va_arg(aq, double);
  struct big b = va_arg(aq, struct big);
  va_end(aq);
  *p = (int)d + b.c;
}

int main(void) {
  int x = 0;
  struct big b = {1, 2, __VERIFIER_nondet_int() % 4};
  store(0, &x, 3.0, b);
  __VERIFIER_assert(x != 6);
  return 0;
}
