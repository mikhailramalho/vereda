/* A pointer that may be NULL keeps its claim. */
#include <stddef.h>

_Bool __VERIFIER_nondet_bool(void);
int x;

int main()
{
  int *p = __VERIFIER_nondet_bool() ? &x : NULL;
  __VERIFIER_assert(p != NULL); // p may be NULL
  return 0;
}
// Imported from ESBMC regression/esbmc/null_cast_fold_fail
// EXPECT: ^VERIFICATION FAILED$
