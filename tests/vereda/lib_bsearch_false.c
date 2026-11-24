// bsearch returns NULL for a key that is not in the array.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

static int cmp(const void *a, const void *b) {
  int x = *(const int *)a, y = *(const int *)b;
  if (x < y)
    return -1;
  return x > y;
}

int main(void) {
  int a[4] = {1, 3, 5, 7};
  int k = __VERIFIER_nondet_int();
  int *p = bsearch(&k, a, 4, sizeof a[0], cmp);
  __VERIFIER_assert(p != NULL);
  return 0;
}
