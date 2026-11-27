// Valid frees: null, a failed allocation's null result, zero-size and
// calloc objects, an object freed in a callee once, frees on disjoint
// branches, an allocation and free per loop iteration, and realloc through
// its null, growing, failing and zero-size paths.
// RUN: --unwind 4
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
#include <stdlib.h>

int __VERIFIER_nondet_int(void);

static void release(void *p) { free(p); }

int main(void) {
  free(NULL);
  char *p = malloc(__VERIFIER_nondet_int() ? 8 : 0);
  release(p);
  free(calloc(3, 4));

  int *q = malloc(sizeof(int));
  if (__VERIFIER_nondet_int())
    free(q);
  else
    release(q);

  for (int i = 0; i < 3; ++i) {
    int *r = malloc(sizeof(int));
    free(r);
  }

  char *s = realloc(NULL, 4);
  char *t = realloc(s, 8);
  if (t)
    s = t;
  if (!realloc(s, SIZE_MAX))
    s = realloc(s, 0);
  free(s);
  return 0;
}
