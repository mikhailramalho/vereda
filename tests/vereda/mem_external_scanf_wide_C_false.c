// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// %2C is %2lc: two wide characters, all 8 bytes of x.
#include <stdio.h>
#include <wchar.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char s[8];
  wchar_t x[2] = {0};
  sscanf(s, "%2C", x);
  __VERIFIER_assert(x[1] == 0);
  return 0;
}
