#include <assert.h>
int main()
{
  int x = 1;
  if (x == 0)
    assert(x != 0);
  assert(x == 1);
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6387_8
// Needs: --assertion-coverage-claims
// RUN: --multi-property --assertion-coverage-claims
// MODE: FUTURE
// EXPECT: ^Total Asserts: 2$
// EXPECT: ^Unreached Asserts: 1$
// EXPECT: ^Reached Assertion Instances: 1$
// EXPECT: ^Assertion Instances Coverage: 50%$
// Vereda: verdict none
