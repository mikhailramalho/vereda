/* free changes memory state no loop variable holds, so the inductive step
   must still follow the paths the bound cuts out of the loop
   (esbmc/esbmc#1320). */
#include <stdlib.h>
int main()
{
  int *p = malloc(sizeof(int));
  unsigned i = 0;
  while (i < 20)
  {
    if (i == 0)
      free(p);
    i++;
  }
  *p = 1;
  return 0;
}
// Imported from ESBMC regression/k-induction/github_1320_free_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 25 --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
