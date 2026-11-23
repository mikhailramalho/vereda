#include <assert.h>

int defined_global = 5;
int tentative_global;        /* tentative definition, not extern */
extern int undefined_extern; /* no definition anywhere: havoc'd */

int main(void)
{
  assert(defined_global == 5);
  assert(tentative_global == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1424
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: no definition for external symbol undefined_extern declared at file .*github_1424\.c line 5
// EXPECT: \A(?!(?s:.)*external symbol defined_global)
// EXPECT: \A(?!(?s:.)*external symbol tentative_global)
// Vereda: unmet EXPECT: no definition for external symbol undefined_extern declared at file .*github_1424\.c line 5
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
