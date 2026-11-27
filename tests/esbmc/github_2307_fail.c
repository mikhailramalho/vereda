#include <assert.h>
#include <string.h>

unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  int arr1[4096];
  int arr2[4096];

  memcpy(arr1 + 1, arr2 + 2, 4094 * sizeof(int));

  unsigned i = __VERIFIER_nondet_uint() % 4094;
  assert(arr1[i + 1] == arr2[i + 1]);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2307_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
