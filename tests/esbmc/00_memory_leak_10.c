//#include <stdio.h>
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
   p[1] = 'z';
   p = malloc(10);
   __VERIFIER_assume(p);
   p[2] = 'z';
//   p[-1] = 'z';
}
// Imported from ESBMC regression/esbmc/00_memory_leak_10
// Needs: --memory-leak-check
// RUN: --memory-leak-check --no-unwinding-assertions --unwind 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
