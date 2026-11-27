// Freeing an object twice is undefined (C17 7.22.3.3p2). malloc may fail
// here, and freeing its null result is valid: only the success path fails,
// on 64 and 32 bits, and with the bound and the solver incremental, whose
// resumed walks keep which objects are dead. The second free in the loop is
// the first to fail.
// RUN: -m64
// RUN: -m32
// RUN: --incremental-bmc
// RUN: --incremental-solver --unwind 3
// EXPECT: ^  dereference failure: invalidated dynamic object freed$
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.1\]  line 18  dereference failure: invalidated dynamic object freed$
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>

int main(void) {
  int *p = malloc(sizeof(int));
  for (int i = 0; i < 2; ++i)
    free(p);
  return 0;
}
