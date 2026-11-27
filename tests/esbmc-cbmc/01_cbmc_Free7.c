#include <stdlib.h>

int main ( void )
{ 
   int i;
   char* p;
   free(p);

}
// Imported from ESBMC regression/cbmc/01_cbmc_Free7
// Needs: --memory-leak-check
// RUN: --memory-leak-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
