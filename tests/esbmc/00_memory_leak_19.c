#include <stdlib.h>

int main()
{

  int *a = (int *) malloc(sizeof(int));

  if(a != NULL)
    return 0;

  free(a);

  return 0;
}
// Imported from ESBMC regression/esbmc/00_memory_leak_19
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
