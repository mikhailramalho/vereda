// A memset whose size has several values writes the bytes below it, an ite
// per byte up to the most the object holds, and leaves the others alone.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  char buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n >= 1 && n <= 4);
  memset(buf, 0, n);
  __VERIFIER_assert(buf[0] == 0);
  __VERIFIER_assert(n < 3 || buf[2] == 0);
  __VERIFIER_assert(n > 2 || buf[2] == 3);
  __VERIFIER_assert(buf[4] == 5);
  return 0;
}
