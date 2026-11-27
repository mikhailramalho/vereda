// A memcpy and a memset of nondet size keep the initialisation of the
// bytes they copy or set, and of those past the size.
// RUN: --ub-uninit-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);

int main(void) {
  char a[4] = {1, 2, 3, 4}, b[4], c[4];
  b[0] = 5;
  b[1] = 6;
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 2);
  memcpy(a, b, n);
  memset(c, 0, n + 1);
  return a[0] + a[1] + a[3] + c[0];
}
