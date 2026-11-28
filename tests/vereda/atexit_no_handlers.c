// EXPECT: ^VERIFICATION SUCCESSFUL$
// _Exit and abort end the program without calling the handlers atexit
// registered (C17 7.22.4.5, 7.22.4.1).
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
static void bye(void) { __VERIFIER_assert(0); }
int main(void) {
  atexit(bye);
  if (__VERIFIER_nondet_int())
    _Exit(0);
  abort();
}
