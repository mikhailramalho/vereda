// RUN: --no-summarize-array-loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
struct entry { int *p; unsigned char flag; };
int main(void) {
  int target = 7;
  struct entry a[16], b[16];
  for (unsigned i = 0; i < 16; ++i) {
    a[i].p = &target;
    a[i].flag = i;
  }
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 16);
  if (__VERIFIER_nondet_uint())
    a[i].flag = 42;
  memcpy(b, a, sizeof b);
  __VERIFIER_assert(b[i].p == &target);
  __VERIFIER_assert(*b[i].p == 7);
  __VERIFIER_assert(b[i].flag == i || b[i].flag == 42);
}
