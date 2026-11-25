// A path bound proves the reconstructed pointer stays in this object.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int a[4];
int main(void) {
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 4);
  int *p = (int *)((uintptr_t)a + sizeof(int) * k);
  *p = 7;
  __VERIFIER_assert(a[k] == 7);
  __VERIFIER_assert((int *)((uintptr_t)a + sizeof(a)) == a + 4);
}
