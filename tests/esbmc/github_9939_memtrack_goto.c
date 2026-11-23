/* #9939: the leaking path jumps past the first return to the second. */
#include <stdlib.h>
int __VERIFIER_nondet_int(void);

int main(void)
{
  int *p = malloc(sizeof(int));
  if (__VERIFIER_nondet_int())
    goto out;
  free(p);
  return 1;
out:
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9939_memtrack_goto
// Needs: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// RUN: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 13\n\s*column: 3$
// Vereda: verdict none
