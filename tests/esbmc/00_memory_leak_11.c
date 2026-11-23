//#include <stdio.h>
#include <stdlib.h>

void really ( void );

int main ( void )
{ 
  char *memoryArea = malloc(10);
  __VERIFIER_assume(memoryArea);
  char *newArea = malloc(10);
  __VERIFIER_assume(newArea);

  memoryArea[3]= newArea;

  free(memoryArea);

  return 0;
}
// Imported from ESBMC regression/esbmc/00_memory_leak_11
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --no-slice --memory-leak-check --unwind 1 -Wno-error=int-conversion --no-reachable-memory-leak --no-abnormal-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
