// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// __VERIFIER_nondet_memory on part of an object makes those bytes nondet.
#include <stddef.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_nondet_memory(void *, size_t);
int main(void) {
  char a[4] = "abc";
  __VERIFIER_nondet_memory(a + 1, 2);
  __VERIFIER_assert(a[2] == 'c');
  return 0;
}
