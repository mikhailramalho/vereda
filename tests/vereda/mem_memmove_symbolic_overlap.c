// A memmove of a nondet size onto an overlapping range reads every source
// byte before writing: a[4] gets the old a[2], both byte by byte (a small
// object) and in the lambda (a large one, the size unbounded).
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
char big[2048];

int main(void) {
  char a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 6);
  memmove(a + 2, a, n);
  __VERIFIER_assert(n < 1 || a[2] == 1);
  __VERIFIER_assert(n < 3 || a[4] == 3);
  __VERIFIER_assert(n >= 6 || a[7] == 8);
  __VERIFIER_assert(a[0] == 1 && a[1] == 2);

  big[0] = 1, big[1] = 2, big[2] = 3;
  unsigned m = __VERIFIER_nondet_uint();
  memmove(big + 2, big, m);
  __VERIFIER_assert(m < 3 || big[4] == 3);
  __VERIFIER_assert(m < 1 || big[2] == 1);
  return 0;
}
