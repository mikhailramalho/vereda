// An uninitialised array, variable-length array or malloc'd block holds one
// unknown value per byte until it is written: two reads of the same element
// agree, and a copy equals its source (array-cav19/array_tiling_tcpy).
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int a[4];
  int x = a[1];
  __VERIFIER_assert(x == a[1]);

  int n = __VERIFIER_nondet_int();
  if (n < 2 || n > 8)
    return 0;
  int v[n], c[n];
  c[n - 1] = v[n - 1];
  __VERIFIER_assert(c[n - 1] == v[n - 1]);

  int *h = malloc(4 * sizeof(int));
  if (!h)
    return 0;
  int y = h[2];
  __VERIFIER_assert(y == h[2]);
  return 0;
}
