/* Read through the same bitcast, a pointer that may be NULL keeps its
 * claim. */
#include <stddef.h>

_Bool __VERIFIER_nondet_bool(void);
int x;

int main()
{
  int *p = __VERIFIER_nondet_bool() ? &x : NULL;
  void *q = *(void **)&p;
  __VERIFIER_assert(q != NULL); // q may be NULL
  return 0;
}
// Imported from ESBMC regression/esbmc/bitcast_pointer_to_pointer_fold_fail
// EXPECT: ^VERIFICATION FAILED$
