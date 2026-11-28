// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^TRUE$
// EXPECT-NOT: UNKNOWN
// Both handlers atexit registers end. Every call of atexit names its
// handler, so the handler call goes to those two only (#479); the walk then
// runs both.
#include <stdlib.h>
static int g;
static void one(void) { g = 1; }
static void two(void) { g = 2; }
int main(void) {
  atexit(one);
  atexit(two);
  return 0;
}
