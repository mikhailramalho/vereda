// wcsdup copies a wide string whose first character is nondet into a new
// object, which free takes, and wcslen counts the copy (#503).
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
#include <wchar.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  wchar_t s[4] = {__VERIFIER_nondet_int(), L'b', L'c', 0};
  wchar_t *d = wcsdup(s);
  if (d) {
    __VERIFIER_assert(d != s);
    __VERIFIER_assert(wcslen(d) == (s[0] ? 3 : 0));
    __VERIFIER_assert(d[0] == s[0]);
    if (s[0])
      __VERIFIER_assert(d[1] == L'b' && d[2] == L'c' && d[3] == 0);
    free(d);
  }
  return 0;
}
