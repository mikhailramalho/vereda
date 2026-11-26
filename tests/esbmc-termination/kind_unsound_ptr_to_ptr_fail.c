/* As kind_unsound_ptr_to_ptr, but the loop never writes **q, so the havoc
 * covers it and the inductive step still proves non-termination. */
#include <stdlib.h>

int main(void)
{
  int x = 0;
  int *p = malloc(sizeof(int));
  if (!p)
    abort();
  int **q = &p;
  *p = 0;
  while (**q < 10)
    x = 1 - x;
  free(p);
  return 0;
}
// Imported from ESBMC regression/termination/kind_unsound_ptr_to_ptr_fail
// Needs: --state-hashing --add-symex-value-sets --termination --max-inductive-step 3 --interval-analysis
// RUN: --state-hashing --add-symex-value-sets --k-step 2 --unlimited-k-steps --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
