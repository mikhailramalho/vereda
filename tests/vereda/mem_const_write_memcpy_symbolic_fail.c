// A memcpy of nondet size into an object defined const writes it whenever
// the size is not zero (C11 6.7.3p6).
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 14  dereference failure: write access to const object `t'$
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
const int t[2] = {1, 2};

int main(void) {
  int s[2] = {7, 8};
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= sizeof t);
  memcpy((void *)t, s, n);
  return 0;
}
