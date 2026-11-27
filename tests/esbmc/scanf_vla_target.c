#include <assert.h>
#include <stdio.h>

int main()
{
  unsigned n;
  __VERIFIER_assume(n == 4);
  char buf[n];
  buf[0] = 'a';
  int r = sscanf("xy", "%s", &buf[1]);
  if (r == 1)
    assert(buf[0] == 'a');
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_vla_target
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
