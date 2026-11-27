// As mem_memcpy_symbolic_pointers_fail, from a local array indexed at a
// nondet offset, which has arrays of its own indexed by offset.
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern void ext(int **);
int x = 1;

int main(void) {
  int *table[300] = {0};
  unsigned n = __VERIFIER_nondet_uint(), i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n >= 1 && n <= 300 && i < n);
  table[i] = &x;
  int **p = malloc(n * sizeof *p);
  if (!p)
    return 0;
  memcpy(p, table, n * sizeof *p);
  ext(p);
  __VERIFIER_assert(x == 1);
  return 0;
}
