#include <stdlib.h>
int main(int argc, char *argv[])
{
  int *arr = (int *)malloc(3 * sizeof(int));
  for(int i = 0; i < 3; i++)
  {
    scanf("%10d", &arr[i]);
  }
  for(int i = 0; i < 3; i++)
  {
    printf("%d", &arr[i]);
  }
}
// Imported from ESBMC regression/cstd/scanf_malloc_3
// RUN: --overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
