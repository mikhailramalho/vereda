#include <stdlib.h>

struct S {
  int x;
  struct S *n;
};

int main(int argc, char *argv[]) {
  struct S *p, *q;

  p=malloc(sizeof(struct S));
  __VERIFIER_assume(p);
  p->x = 5;
  p->n = NULL;
  
  argc=__VERIFIER_nondet_int();

  if (argc > 1) {
    q = malloc(sizeof(struct S));
    __VERIFIER_assume(q);
    q->x = 5;
    q->n = p;
  } else {
    q = p;
  }
  
  assert(p->x + q->x == 10);
  
  free(q);
  free(p);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Free5
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
