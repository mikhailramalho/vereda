#include <stdlib.h>
int main(int argc, char *argv[])
{
  int *arr = (int *)malloc(3 * sizeof(int));
  for(int i = 0; i < 3; i++)
  {
    scanf("%13d", &arr[i]);
  }
  for(int i = 0; i < 3; i++)
  {
    printf("%d", &arr[i]);
  }
}
// Imported from ESBMC regression/cstd/scanf_malloc_bug_3
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
