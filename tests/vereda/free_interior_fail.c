// free takes the start of an object only (C17 7.22.3.3p2): a pointer into
// its middle is reported as ESBMC reports it.
// RUN: --force-malloc-success
// EXPECT: ^  Operand of free must have zero pointer offset$
// EXPECT: ^  FAILED       \[main\.free-non-zero-offset\.1\]  line 11  Operand of free must have zero pointer offset$
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>

int main(void) {
  char *p = malloc(8);
  free(p + 4);
  return 0;
}
