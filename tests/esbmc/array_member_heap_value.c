#include <stdlib.h>
typedef struct { unsigned a; unsigned arr[4]; unsigned z; } S;
int main(void)
{
  S *p = malloc(sizeof(S));
  if (!p) return 0;
  p->a = 0; p->z = 0;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 4);
  unsigned *q = &p->arr[k];
  *q = 9;
  __VERIFIER_assert(p->arr[k] == 9 && p->a == 0 && p->z == 0); // heap value
  return 0;
}
// Imported from ESBMC regression/esbmc/array_member_heap_value
// EXPECT: ^VERIFICATION SUCCESSFUL$
