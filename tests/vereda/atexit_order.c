// EXPECT: ^VERIFICATION SUCCESSFUL$
// exit and the return of main call the handlers atexit registered, last
// first: with code 0, exit runs first alone; otherwise the return runs
// second, then first (C17 7.22.4.4p3, 5.1.2.2.3).
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
static int order, code;
static void first(void) {
  order = order * 10 + 1;
  __VERIFIER_assert(order == (code ? 21 : 1));
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
