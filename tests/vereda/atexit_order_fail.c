// EXPECT: ^VERIFICATION FAILED$
// As atexit_order.c, but first expects 21 always: with code 0, exit runs
// first alone and order is 1.
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
static int order, code;
static void first(void) {
  order = order * 10 + 1;
  __VERIFIER_assert(order == 21);
}
static void second(void) { order = order * 10 + 2; }
int main(void) {
  code = __VERIFIER_nondet_int();
  atexit(first);
  if (!code)
    exit(0);
  atexit(second);
  return 0;
}
