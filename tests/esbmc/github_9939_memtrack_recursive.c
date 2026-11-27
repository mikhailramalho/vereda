/* #9939: main can re-enter itself, so its last step need not be in its outer call and the target stays at the brace. */
#include <stdlib.h>
int d;
void f(void);
int main(void)
{
  if (d == 0)
  {
    d = 1;
    f();
    return 1;
  }
  int *p = malloc(sizeof(int));
  p = 0;
  return 0;
}
void f(void)
{
  main();
}
// Imported from ESBMC regression/esbmc/github_9939_memtrack_recursive
// Needs: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml
// RUN: --memory-leak-check --no-reachable-memory-leak --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: forgotten memory
// EXPECT: type: "target"\n.*\n.*\n.*\n\s*line: 16\n\s*column: 1$
// Vereda: verdict none
