// qsort moves x from the front whenever x is not the least of the three.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static int cmp(const void *a, const void *b) {
  int x = *(const int *)a, y = *(const int *)b;
  if (x < y)
    return -1;
  return x > y;
}

int main(void) {
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int(), z = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= -100 && x <= 100 && y >= -100 && y <= 100 && z >= -100 && z <= 100);
  int a[3] = {x, y, z};
  qsort(a, 3, sizeof a[0], cmp);

  __VERIFIER_assert(a[0] == x);
  return 0;
}
