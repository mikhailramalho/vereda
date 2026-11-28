// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// The handler atexit registered runs forever after main returns.
#include <stdlib.h>
static void forever(void) {
  for (;;)
    ;
}
int main(void) {
  atexit(forever);
  return 0;
}
