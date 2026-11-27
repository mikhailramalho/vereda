#include <assert.h>
int main()
{
  int x;
  assert(x != 1);
  assert(x != 2);
  assert(x != 3);
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6387_11
// Needs: --multi-fail-fast 1 --assertion-coverage
// RUN: --multi-property --multi-fail-fast 1 --assertion-coverage --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^COVERAGE ANALYSIS INCOMPLETE: 2 goal\(s\) undecided
// EXPECT: reason: --multi-fail-fast limit reached
// Vereda: verdict none
