// RUN: --unwind 2
// The walker compares memcmp's and bcmp's ranges whole (#67): --unwind 2
// would stop a byte loop long before 64. The sign comes from the first
// differing byte as an unsigned char; n may be symbolic, and so may the
// contents.
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern unsigned char __VERIFIER_nondet_uchar(void);
int main(void) {
  unsigned char a[64], b[64];
  memset(a, 7, sizeof a);
  memset(b, 7, sizeof b);
  a[40] = 0x01;
  b[40] = 0x80;
  assert(memcmp(a, b, 40) == 0);
  assert(memcmp(a, b, 64) < 0);
  assert(memcmp(b, a, 64) > 0);
  assert(bcmp(a, b, 40) == 0 && bcmp(a, b, 64) != 0);
  unsigned n = __VERIFIER_nondet_uint() % 65;
  assert((memcmp(a, b, n) == 0) == (n <= 40));
  unsigned char x[3], y[3];
  x[0] = __VERIFIER_nondet_uchar();
  x[1] = __VERIFIER_nondet_uchar();
  x[2] = __VERIFIER_nondet_uchar();
  y[0] = __VERIFIER_nondet_uchar();
  y[1] = __VERIFIER_nondet_uchar();
  y[2] = __VERIFIER_nondet_uchar();
  int r = memcmp(x, y, 3);
  if (r == 0)
    assert(x[0] == y[0] && x[1] == y[1] && x[2] == y[2]);
  if (x[0] < y[0])
    assert(r < 0);
  return 0;
}
// EXPECT: ^VERIFICATION SUCCESSFUL$
