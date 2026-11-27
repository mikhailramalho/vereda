#include <assert.h>
#include <stdio.h>

int main()
{
  int arr[3] = {1, 2, 3};
  unsigned i;
  __VERIFIER_assume(i < 3);
  int r = sscanf("5", "%d", &arr[i]);
  if (r == 1)
    assert(arr[i] == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_symbolic_index_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
