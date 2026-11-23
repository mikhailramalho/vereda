/* #9939: without a return, a memtrack target stays at main's closing brace. */
#include <stdlib.h>

int main(void)
{
  int *p = malloc(sizeof(int));
  p = 0;
}
// Imported from ESBMC regression/esbmc/github_9939_memtrack_no_return
// Needs: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// RUN: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 8\n\s*column: 1$
// Vereda: verdict none
