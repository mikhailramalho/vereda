#include <stdlib.h>
#include <stdio.h>

int main(void) {
  char *chptr;
  char *chptr1;
  int i = 1;
  chptr = (char *) malloc(12);
  __VERIFIER_assume(chptr);
  chptr1 = (char *) malloc (12);
  __VERIFIER_assume(chptr1);
  for ( i; i <= 13; i++ ) {
    chptr[i] = '?';        /* error when i = 13 invalid write */
    chptr1[i] = chptr[i];  /* error when i = 13 invalid read and write */
  }

  free(chptr1);
  free(chptr);
}
// Imported from ESBMC regression/esbmc/00_memory_leak_05
// Needs: --memory-leak-check
// RUN: --memory-leak-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
