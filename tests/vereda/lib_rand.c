// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// rand returns a value from 0 to RAND_MAX.
#include <stdlib.h>
extern void __VERIFIER_assert(int);
int main(void) {
  srand(1);
  int r = rand();
  __VERIFIER_assert(r >= 0 && r <= RAND_MAX);
  return 0;
}
