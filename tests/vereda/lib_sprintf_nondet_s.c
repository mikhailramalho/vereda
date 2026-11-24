// %s of a nondet buffer copies it up to its first NUL, wherever that is.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdio.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_nondet_memory(void *, unsigned long);
int main(void) {
  char src[6], dst[8];
  __VERIFIER_nondet_memory(src, sizeof src);
  src[5] = 0;
  int n = sprintf(dst, "<%s", src);
  __VERIFIER_assert(n >= 1 && n <= 6);
  __VERIFIER_assert(dst[n] == 0);
  for (int i = 1; i < n; ++i)
    __VERIFIER_assert(dst[i] == src[i - 1] && dst[i] != 0);
  return 0;
}
