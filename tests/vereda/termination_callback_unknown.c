// RUN: --property-file %S/properties/termination.prp
// EXPECT: ^VERIFICATION UNKNOWN: call to signal .*, which may call a function whose address is taken$
// EXPECT: ^UNKNOWN$
// The signal handler may run forever; the walk would not see it.
#include <signal.h>

static void forever(int s) {
  for (;;)
    ;
}

int main(void) {
  signal(SIGINT, forever);
  return 0;
}
