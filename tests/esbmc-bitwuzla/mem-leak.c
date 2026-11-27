#include <stdio.h>
#include <stdlib.h>

int main()
{
  int *i;
  i = (int *)malloc(10);

  //free(i);
  //free(i);

  return 0;
}
// Imported from ESBMC regression/bitwuzla/mem-leak
// Needs: --memory-leak-check
// RUN: --bitwuzla --memory-leak-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
