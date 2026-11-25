// A large partial fill (past MaxBytes) uses a partial-range lambda over the
// arrays below instead of millions of byte stores.
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
#include <string.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned char *p = malloc(16u << 20);
  p[0] = 42;
  memset(p + 1, 255, (16u << 20) - 1);
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i >= 1 && i < (16u << 20));
  __VERIFIER_assert(p[0] == 42 && p[1] == 255 && p[i] == 255);
  return 0;
}
