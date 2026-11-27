// A pointer made from an integer that no object holds is no allocation;
// freeing it is an invalid pointer freed, as ESBMC words it.
// EXPECT: ^  dereference failure: invalid pointer freed$
// EXPECT: ^  FAILED       \[main\.invalid-pointer-freed\.1\]  line 9  dereference failure: invalid pointer freed$
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>

int main(void) {
  free((void *)16);
  return 0;
}
