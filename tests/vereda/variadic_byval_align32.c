// A struct aligned to 32 passed byval through ... on x86-64 goes on the
// stack aligned to 32, between two longs, after the six named ints have
// taken the registers: clang's va_arg code finds it only if the stack
// area is aligned to 32 too.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdarg.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

struct big {
  _Alignas(32) int x[4];
};

int pick(int n, int p1, int p2, int p3, int p4, int p5, ...) {
  va_list ap;
  va_start(ap, p5);
  long a = va_arg(ap, long);
  struct big b = va_arg(ap, struct big);
  long c = va_arg(ap, long);
  va_end(ap);
  return b.x[n] + a * 100 + c * 1000;
}

int main(void) {
  int n = __VERIFIER_nondet_int();
  if (n < 0 || n > 3)
    return 0;
  struct big b = {{1, 2, 3, 4}};
  __VERIFIER_assert(pick(n, 1, 2, 3, 4, 5, 7L, b, 9L) == 9701 + n);
  return 0;
}
