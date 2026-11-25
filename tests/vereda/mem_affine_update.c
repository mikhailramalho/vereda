// A later write overrides the compact table, including values wrapping at 64 bits.
// RUN: --no-summarize-array-loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  uint64_t a[256];
  for (unsigned i = 0; i < 256; ++i)
    a[i] = UINT64_C(7) - UINT64_C(3) * i;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 256);
  __VERIFIER_assert(a[k] == UINT64_C(7) - UINT64_C(3) * k);
  a[10] = 99;
  __VERIFIER_assert(a[k] == (k == 10 ? 99 : UINT64_C(7) - UINT64_C(3) * k));
}
