#include <stdlib.h>
#include <stdio.h>

void get_mem() {
  char *ptr;
  ptr = (char *) malloc (10);  /* memory not freed */
  __VERIFIER_assume(ptr);
}

int main(void) {
  int i;
  char *ptr1, *ptr2;
  ptr1 = (char *) malloc (512);
  __VERIFIER_assume(ptr1);
  ptr2 = (char *) malloc (512);
  __VERIFIER_assume(ptr2);
  ptr2 = ptr1; /* causes the memory leak of ptr1 */
  free(ptr2);
  free(ptr1);
  for ( i = 0; i < 512; i++ ) {
    get_mem();
  }
}
// Imported from ESBMC regression/esbmc/00_memory_leak_09
// Needs: --memory-leak-check
// RUN: --no-slice --memory-leak-check --unwind 1
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
