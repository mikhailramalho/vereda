// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// __VERIFIER_nondet_memory gives the whole struct nondet contents.
#include <stddef.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_nondet_memory(void *, size_t);
struct A { int x, y; };
int main(void) {
  struct A a = {0, 0};
  __VERIFIER_nondet_memory(&a, sizeof a);
  __VERIFIER_assert(a.x == a.y);
  return 0;
}
