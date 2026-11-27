/* #10081: --force-malloc-success makes calloc succeed under --sv-comp too. */
#include <stdlib.h>
extern void reach_error(void);
int main()
{
  int *p = calloc(4, sizeof(int));
  if (p == 0)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10081
// Needs: --sv-comp
// RUN: --sv-comp --force-malloc-success --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
