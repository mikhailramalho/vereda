/* #9939: a loop lies between the dropped branch and the return, so the target stays at the brace. */
#include <stdlib.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int *p = malloc(sizeof(int));
  p = 0;
  int x = 0;
  if (__VERIFIER_nondet_int())
    x = 1;
  int i = 0;
  do
  {
    i++;
    x += i;
  } while (i < 2);
  return x;
}
// Imported from ESBMC regression/esbmc/github_9939_memtrack_loop_ahead
// Needs: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// RUN: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 18\n\s*column: 1$
// Vereda: verdict none
