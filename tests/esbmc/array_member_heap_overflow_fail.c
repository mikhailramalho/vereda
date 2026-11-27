#include <stdlib.h>
typedef struct { unsigned a; unsigned arr[4]; unsigned z; } S;
int main(void)
{
  S *p = malloc(sizeof(S));
  if (!p)
    return 0;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k <= 4);
  unsigned *q = &p->arr[k];
  *q = 1;               /* k == 4 writes past arr into z */
  return 0;
}
// Imported from ESBMC regression/esbmc/array_member_heap_overflow_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  dereference failure: array bounds violated: heap object$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: array bounds violated: heap object$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
