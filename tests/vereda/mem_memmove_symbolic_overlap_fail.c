// After the move, a[5] holds the old a[3], not the old a[1].
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 14  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  char a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 6);
  memmove(a + 2, a, n);
  __VERIFIER_assert(n < 4 || a[5] == 2);
  return 0;
}
