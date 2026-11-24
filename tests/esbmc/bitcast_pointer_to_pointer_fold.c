/* A pointer read through a pointer to another pointer type is a bitcast
 * between two pointer types. It still names the object it pointed at, so
 * both claims fold without the solver. */
#include <stddef.h>

int x;

int main()
{
  int *p = &x;
  void *q = *(void **)&p;
  __VERIFIER_assert(q != NULL); // q is not NULL
  __VERIFIER_assert(__ESBMC_same_object(q, &x)); // q points at x
  return 0;
}
// Imported from ESBMC regression/esbmc/bitcast_pointer_to_pointer_fold
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
