// One past the end of an array equals the start of another array only when
// that one follows it with no gap (C11 6.5.9p6). Vereda lays objects out
// with at least one byte between them, so that never happens here, while one
// past the end does equal itself made from its address.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
extern void __VERIFIER_assert(int);
int a[2], b[2];

int main(void) {
  __VERIFIER_assert(a + 2 != b);
  __VERIFIER_assert(b + 2 != a);
  int *end = (int *)((uintptr_t)a + sizeof a);
  __VERIFIER_assert(end == a + 2);
  return 0;
}
