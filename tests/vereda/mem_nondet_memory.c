// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// __VERIFIER_nondet_memory writes only the n bytes it is given.
#include <stddef.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_nondet_memory(void *, size_t);
int main(void) {
  char a[4] = "abc", b[4] = "xyz";
  int c = __VERIFIER_nondet_int();
  __VERIFIER_nondet_memory(c ? a + 1 : b, 2);
  __VERIFIER_assert(a[0] == 'a' && a[3] == 0);
  __VERIFIER_assert(b[2] == 'z' && b[3] == 0);
  if (c)
    __VERIFIER_assert(b[0] == 'x');
  else
    __VERIFIER_assert(a[1] == 'b');
  return 0;
}
