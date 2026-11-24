// A null pointer read back from zeroed memory stays null on a path where a
// malloc failed: the address 0 converts to no object that path never
// allocated, whose base is free there (#137, the LDV final-state checks).
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern void __VERIFIER_assert(int);

int *last;

int main(void) {
  int *p = malloc(sizeof *p);
  if (!p)
    __VERIFIER_assert(last == 0);
  return 0;
}
