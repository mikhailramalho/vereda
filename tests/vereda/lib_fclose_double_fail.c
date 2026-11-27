// Closing an fopen stream twice is a double free.
// EXPECT: ^  dereference failure: invalidated dynamic object freed$
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.1\]  line 11  dereference failure: invalidated dynamic object freed$
// EXPECT: ^VERIFICATION FAILED$
#include <stdio.h>
int main(void) {
  FILE *f = fopen("x", "r");
  if (!f)
    return 0;
  fclose(f);
  fclose(f);
  return 0;
}