// The object iteration 50 allocates is lost in iteration 60. The leak check at
// the end of the program covers only the objects symex allocated, and the
// inductive step starts from a havoced state past that allocation, so it would
// prove the leak absent. A loop that allocates disables the step under a leak
// check, and the base case finds the leak.
#include <stdlib.h>

int *g;

int main(void)
{
  for (int i = 0; i < 100; i++)
  {
    if (i == 50)
      g = malloc(sizeof(int));
    if (i == 60)
      g = 0;
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/leak_lost_in_loop_memtrack_fail
// Needs: --k-induction --max-inductive-step 3 --memory-leak-check --no-reachable-memory-leak
// RUN: --k-induction --max-inductive-step 3 --k-step 2 --unlimited-k-steps --memory-leak-check --no-reachable-memory-leak --no-assertions
// MODE: FUTURE
// EXPECT: cannot check for leaks the objects a loop allocated
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
