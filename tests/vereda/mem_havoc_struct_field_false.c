// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.invalid-pointer-freed\.1\]
// pick() returns { int **, long } with the long set only where keep holds,
// joined as ite(keep, constant, concat(nondet, null)).
// There the pointer field is &b, so the call without a body reaches b's slot
// through v and may leave any pointer in it: freeing b is then invalid.
// Losing &b from the field would lose that reach (#499).
#include <stdlib.h>
int __VERIFIER_nondet_int(void);
struct r { int **q; long n; };
static struct r pick(int **q, int keep) {
  struct r x;
  x.q = 0;
  if (keep) {
    x.q = q;
    x.n = 4;
  }
  return x;
}
void touch(int ***q);
int main(void) {
  int *b = malloc(sizeof(int));
  int **v = pick(&b, __VERIFIER_nondet_int()).q;
  touch(&v);
  free(b);
  return 0;
}
