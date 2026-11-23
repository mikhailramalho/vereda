/* #9939: the path follows the jump past the else branch to the last return. */
#include <stdlib.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int *p = malloc(sizeof(int));
  if (__VERIFIER_nondet_int())
    p = 0;
  else
  {
    free(p);
    return 1;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9939_memtrack_forward_goto
// Needs: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// RUN: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 14\n\s*column: 3$
// Vereda: verdict none
