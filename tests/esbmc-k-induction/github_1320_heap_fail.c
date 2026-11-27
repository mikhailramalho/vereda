/* strcpy writes the heap buffer through a pointer the havoc does not cover,
   so the inductive step must still follow the paths the bound cuts out of its
   loop, or it would prove the claim before the base case reaches the
   violation (esbmc/esbmc#1320). */
#include <stdlib.h>
#include <string.h>

int main()
{
  char *d = calloc(7, 1);
  strcpy(d, "abcdef");
  __VERIFIER_assert(d[0] != 'a'); // strcpy copies the first byte
  return 0;
}
// Imported from ESBMC regression/k-induction/github_1320_heap_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 10 --ub-pointer-check=false --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
