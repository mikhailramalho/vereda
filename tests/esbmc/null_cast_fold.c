/* `p != NULL` against a `T *` reaches the simplifier as `(T *)NULL`. The
 * non-null fold has to recognise it, or the claim goes to the solver. */
#include <stddef.h>

struct node
{
  unsigned long size;
};

int main()
{
  struct node *p = __builtin_alloca(sizeof(struct node));
  __VERIFIER_assert(p != NULL); // an allocation is not NULL
  return 0;
}
// Imported from ESBMC regression/esbmc/null_cast_fold
// EXPECT: ^VERIFICATION SUCCESSFUL$
