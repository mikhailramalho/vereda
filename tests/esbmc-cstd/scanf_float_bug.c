#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
  float *arr = (float *)malloc(3 * sizeof(float));
  for(int i = 0; i < 3; i++)
  {
    scanf("%13f", &arr[i]);
  }
  for(int i = 0; i < 3; i++)
  {
    printf("%f", &arr[i]);
  }
}
// Imported from ESBMC regression/cstd/scanf_float_bug
// RUN: --overflow-check
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict SUCCESSFUL
