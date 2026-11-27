// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
struct entry { unsigned char x; unsigned short y; };
int main(void) {
  struct entry a[256], b[256];
  memset(a, 0, sizeof a);
  for (unsigned i = 0; i < 256; ++i) {
    a[i].x = i % 7;
    a[i].y = 3 * i;
  }
  memcpy(b, a, sizeof b);
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 256);
  __VERIFIER_assert(b[i].x == i % 7);
  __VERIFIER_assert(b[i].y == 3 * i);
  memset(b, 0, sizeof b);
  __VERIFIER_assert(b[i].x == 0);
  __VERIFIER_assert(b[i].y == 0);
}
