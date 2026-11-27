/* #9939: a void main has no return statement, so the target stays at the brace. */
#include <stdlib.h>
void main(void)
{
  int *p = malloc(sizeof(int));
  p = 0;
}
// Imported from ESBMC regression/esbmc/github_9939_memtrack_void_main
// Needs: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml
// RUN: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 7\n\s*column: 1$
// Vereda: verdict none
