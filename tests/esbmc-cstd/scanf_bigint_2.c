#include <stdlib.h>
int main(int argc, char *argv[])
{
  int *arr = (int *)malloc(100000000000 * sizeof(int));
  for(int i = 0; i < 3; i++)
  {
    scanf("%100000000000d", &arr[i]);
  }
  for(int i = 0; i < 3; i++)
  {
    printf("%d", &arr[i]);
  }
}
// Imported from ESBMC regression/cstd/scanf_bigint_2
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
