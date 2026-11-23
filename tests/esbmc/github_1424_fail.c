#include <assert.h>

extern int undefined_extern;

int main(void)
{
  assert(undefined_extern == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1424_fail
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: no definition for external symbol undefined_extern declared at file .*github_1424_fail\.c line 3
// Vereda: unmet EXPECT: no definition for external symbol undefined_extern declared at file .*github_1424_fail\.c line 3
// MODE: FUTURE
// Vereda: verdict FAILED
