#include <stdlib.h>

typedef struct
{
  int p;
} T;

int main(void)
{
  T *t = malloc(sizeof(T));
  if (!t)
    return 0;

  int v = __VERIFIER_nondet_int();
  if (v != 0)
  {
    t->p = 1;
  }

  /* Nothing writes t->p when v == 0, so this must not be provable. */
  __VERIFIER_assert(t->p == 1); // conditionally written heap object
  free(t);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6798
// RUN: --unwind 4
// EXPECT: ^VERIFICATION FAILED$
