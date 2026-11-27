// RUN: --property-file %S/properties/termination.prp
// EXPECT: ^VERIFICATION UNKNOWN: call to atexit .*, which may call a function whose address is taken$
// EXPECT: ^UNKNOWN$
// atexit runs forever after main returns; the walk would not see it.
#include <stdlib.h>

static void forever(void) {
  for (;;)
    ;
}

int main(void) {
  atexit(forever);
  return 0;
}
