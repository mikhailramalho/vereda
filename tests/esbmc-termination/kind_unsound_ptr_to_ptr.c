/* The loop counts through **q. k-induction cannot havoc that cell, so its
 * inductive step starts where the loop started and does not reach the exit
 * within k iterations; that is no proof of non-termination. The write to x
 * keeps the loop's modified set non-empty, so only that havoc gate can
 * disable the inductive step. */
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
  {
    x = 1 - x;
    **q = **q + 1;
  }
  free(p);
  return 0;
}
// Imported from ESBMC regression/termination/kind_unsound_ptr_to_ptr
// Needs: --termination --max-inductive-step 3 --interval-analysis
// RUN: --k-step 2 --unlimited-k-steps --ub-pointer-check=false --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
