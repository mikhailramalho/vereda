
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Simple test program, has a race.  Parent and child both modify x
   with no locking. */

void __ESBMC_yield();
int x = 0;

void* child_fn ( void* arg )
{
   /* Unprotected relative to parent */
   x++;
   return NULL;
}

int main ( void )
{
   int i;
   pthread_t child;

   if (pthread_create(&child, NULL, child_fn, NULL)) {
      perror("pthread_create");
      exit(1);
   }


   /* Unprotected relative to child */
   x++;

   if (pthread_create(&child, NULL, child_fn, NULL)) {
      perror("pthread_create");
      exit(1);
   }

   if (pthread_join(child, NULL)) {
      perror("pthread join");
      exit(1);
   }

   return 0;
}
// Imported from ESBMC regression/esbmc-unix/00_race14
// Needs: --ub-data-race-check --context-bound 3
// RUN: --ub-data-race-check --context-bound 3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
