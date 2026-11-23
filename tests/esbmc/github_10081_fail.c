/* #10081: without --force-malloc-success, calloc may still fail under --sv-comp. */
#include <stdlib.h>
extern void reach_error(void);
int main()
{
  int *p = calloc(4, sizeof(int));
  if (p == 0)
    reach_error();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10081_fail
// Needs: --sv-comp
// RUN: --sv-comp --no-pointer-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.1\]\s+line\s+8\s+reach_error$
// Vereda: verdict none
