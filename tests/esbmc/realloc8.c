#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

int num, ind, newsize, i = 1;

void *expandArray(int **pointer) {
  while (newsize < num) {
    newsize = newsize + 1;
    int *temp = realloc(*pointer, sizeof(int) * newsize);
    if (temp != NULL) {
      temp[newsize - 1] = i;
      *pointer = temp;
    } else {
      return 0;
    }
  }

  return 0;
}

int main(int argc, char **argv) {
  num = __VERIFIER_nondet_int();
  if (!(num > 0 && num < 100)) {
    return 0;
  }
  int *a = (int *)malloc(sizeof(int));
  if (a == NULL) {
    return 0;
  }
  newsize = 0;
  expandArray(&a);
  free(a);
  return 0;
}
// Imported from ESBMC regression/esbmc/realloc8
// Needs: --force-realloc-success
// RUN: --force-realloc-success --force-malloc-success main.c --incremental-bmc --max-k-step 6
// FILES: main.c
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
