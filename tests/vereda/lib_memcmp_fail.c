// RUN: --unwind 2
// lib_memcmp's results are not vacuous: memcmp of ranges that differ at
// byte 40 is not 0 for every symbolic n.
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned char a[64], b[64];
  memset(a, 7, sizeof a);
  memset(b, 7, sizeof b);
  b[40] = 8;
  unsigned n = __VERIFIER_nondet_uint() % 65;
  assert(memcmp(a, b, n) == 0);
  return 0;
}
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: unwinding assertion
