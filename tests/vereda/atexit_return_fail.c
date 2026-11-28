// EXPECT: ^VERIFICATION FAILED$
// The return of main calls the handler, which fails for x == 3 only.
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
static int x;
static void bye(void) { __VERIFIER_assert(x != 3); }
int main(void) {
  x = __VERIFIER_nondet_int();
  atexit(bye);
  return 0;
}
