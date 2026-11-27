#include <assert.h>
#include <stdio.h>

char big[300];

int main()
{
  sscanf("x", "%s", &big[100]);
  assert(big[0] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_tail_under_cap
// EXPECT: ^VERIFICATION SUCCESSFUL$
