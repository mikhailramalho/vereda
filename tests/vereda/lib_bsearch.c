// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int __vereda_bsearch_cmp(const void *a, const void *b);

int main(void) {
  int a[4] = {1, 3, 5, 7};
  int k = __VERIFIER_nondet_int();
  int *p = bsearch(&k, a, 4, sizeof a[0], __vereda_bsearch_cmp);
  __VERIFIER_assert(p == (k == 1 ? a : k == 3 ? a + 1 : k == 5 ? a + 2 : k == 7 ? a + 3 : NULL));
  return 0;
}