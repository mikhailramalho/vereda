/* #9939: a branch with no recorded direction leads to several returns, so the target stays at the brace. */
#include <stdlib.h>
int main(void)
{
  int *p = malloc(sizeof(int));
  if (!p)
    return 2;
  if (*p)
    return 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9939_memtrack_two_returns
// Needs: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml
// RUN: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 11\n\s*column: 1$
// Vereda: verdict none
