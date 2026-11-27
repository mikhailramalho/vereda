// Pointers a memcpy of nondet size copies into an object without arrays of
// its own are that object's: a call without a body that gets it may follow
// them and write x.
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern void ext(int **);
int x = 1;
int *table[300];

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n >= 1 && n <= 300);
  table[0] = &x;
  int **p = malloc(n * sizeof *p);
  if (!p)
    return 0;
  memcpy(p, table, n * sizeof *p);
  ext(p);
  __VERIFIER_assert(x == 1);
  return 0;
}
