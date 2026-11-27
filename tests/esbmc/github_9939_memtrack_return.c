/* #9939: a memtrack target points at the return the path left main by. */
#include <stdlib.h>
int __VERIFIER_nondet_int(void);

int main(void)
{
  int *p = malloc(sizeof(int));
  if (__VERIFIER_nondet_int())
  {
    free(p);
    return 1;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9939_memtrack_return
// Needs: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml
// RUN: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 13\n\s*column: 3$
// Vereda: verdict none
