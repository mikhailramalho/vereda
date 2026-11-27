#include <assert.h>
#include <stdio.h>

char big[300];

int main()
{
  sscanf("x", "%s", &big[1]);
  assert(big[0] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_tail_beyond_cap_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
