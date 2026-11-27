// %s of an array that may hold no NUL stops at the array's end, as ESBMC's
// model does: the text is at most as long as the bytes (or wide characters,
// for %ls) from the pointer to the end of the array, and otherwise ends at
// the first NUL.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdio.h>
#include <wchar.h>
extern void __VERIFIER_assert(int);
extern void __VERIFIER_nondet_memory(void *, unsigned long);
int main(void) {
  char src[4], dst[16];
  wchar_t w[2];
  __VERIFIER_nondet_memory(src, sizeof src);
  __VERIFIER_nondet_memory(w, sizeof w);
  int n = sprintf(dst, "<%s", src);
  __VERIFIER_assert(n >= 1 && n <= 5);
  __VERIFIER_assert(n == 5 || src[n - 1] == 0);
  __VERIFIER_assert(sprintf(dst, "<%s", src + 1) <= 4);
  __VERIFIER_assert(sprintf(dst, "<%ls", w) <= 3);
  return 0;
}
