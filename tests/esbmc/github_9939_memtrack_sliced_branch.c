/* #9939: the slicer drops the branch, and one return is left ahead. */
#include <stdlib.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int *p = malloc(sizeof(int));
  p = 0;
  int x = 0;
  if (__VERIFIER_nondet_int())
    x = 1;
  return x;
}
// Imported from ESBMC regression/esbmc/github_9939_memtrack_sliced_branch
// Needs: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// RUN: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 11\n\s*column: 3$
// Vereda: verdict none
