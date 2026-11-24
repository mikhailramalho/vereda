// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// %2S is %2ls: two wide characters and a null, all 12 bytes of x.
#include <stdio.h>
#include <wchar.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char s[8];
  wchar_t x[3] = {0};
  sscanf(s, "%2S", x);
  __VERIFIER_assert(x[2] == 0);
  return 0;
}
