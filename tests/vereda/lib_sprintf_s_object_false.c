// "<%s" of a 4-byte array with no NUL in it gives 5 characters, so this
// assertion fails only when none of the 4 bytes is 0.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <stdio.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_nondet_memory(void *, unsigned long);
int main(void) {
  char src[4], dst[16];
  __VERIFIER_nondet_memory(src, sizeof src);
  __VERIFIER_assert(sprintf(dst, "<%s", src) <= 4);
  return 0;
}
