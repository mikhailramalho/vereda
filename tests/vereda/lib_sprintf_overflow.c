// sprintf writes each byte through the buffer, so a text longer than it
// writes past its end. Nothing checks that write yet: it needs the valid
// dereference checks of issue #29.
// EXPECT: ^VERIFICATION FAILED$
// MODE: KNOWNBUG
#include <stdio.h>
int main(void) {
  char b[4];
  sprintf(b, "%s", "hello");
  return 0;
}
