#include <assert.h>
#include <stdio.h>

int main()
{
  int arr[3] = {1, 2, 3};
  int r = sscanf("5", "%d", &arr[1]);
  if (r == 1)
    assert(arr[1] == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_array_elem_store_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
