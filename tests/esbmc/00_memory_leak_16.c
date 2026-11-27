#include <stdlib.h>

void really ( void );

int main ( void )
{ 
   really();
   return 0;
}

void really ( void )
{
   int i;
   char* p = malloc(10);
   __VERIFIER_assume(p);
   for (i = 0; i < 1; i++)
      p[i] = 'z';
   free(p);
//   p[1] = 'z';
   p = malloc(10);
   __VERIFIER_assume(p);
   p[2] = 'z';
   free(p);
}
// Imported from ESBMC regression/esbmc/00_memory_leak_16
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
