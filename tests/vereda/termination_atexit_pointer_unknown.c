// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// One call of atexit names its handler, the other passes a pointer that may
// be spin, which never ends: the handler call keeps every target.
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
static int g;
static void one(void) { g = 1; }
static void spin(void) {
  for (;;)
    ;
}
int main(void) {
  void (*p)(void) = __VERIFIER_nondet_int() ? one : spin;
  atexit(one);
  atexit(p);
  return 0;
}
