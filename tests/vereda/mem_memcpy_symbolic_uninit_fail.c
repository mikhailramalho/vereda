// A memcpy of nondet size copies whether each byte below the size is
// initialised: a[1] holds an uninitialised byte when n is 2.
// RUN: --ub-uninit-check
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);

int main(void) {
  char a[4] = {1, 2, 3, 4}, b[4];
  b[0] = 5;
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 4);
  memcpy(a, b, n);
  return a[0] + a[1];
}
