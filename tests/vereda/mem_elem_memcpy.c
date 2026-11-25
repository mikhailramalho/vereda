// Aligned memcpy, memmove and memset on an element array at constant and
// symbolic offsets, both whole-object and partial.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  int a[8], b[4];
  for (int k = 0; k < 4; ++k)
    b[k] = 10 + k;

  // Whole-object memset, then partial aligned memcpy at constant offset.
  memset(a, 0, sizeof(a));
  memcpy(&a[4], b, sizeof(b));

  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4);
  __VERIFIER_assert(a[i] == 0);
  __VERIFIER_assert(a[4 + i] == (int)(10 + i));

  // Overlapping aligned memmove within the element array.
  memmove(&a[2], &a[4], sizeof(b));
  __VERIFIER_assert(a[2 + i] == (int)(10 + i));

  // Partial aligned memset and memcpy at symbolic element offsets.
  unsigned s = __VERIFIER_nondet_uint();
  __VERIFIER_assume(s <= 4);
  memset(&a[s], 0, 2 * sizeof(int));
  __VERIFIER_assert(a[s] == 0 && a[s + 1] == 0);
  memcpy(&a[s], b, 2 * sizeof(int));
  __VERIFIER_assert(a[s] == 10 && a[s + 1] == 11);
  return 0;
}
